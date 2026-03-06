/************************************************************************
    MeOS - Orienteering Software
    Copyright (C) 2009-2026 Melin Software HB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Melin Software HB - software@melin.nu - www.melin.nu
    Eksoppsvägen 16, SE-75646 UPPSALA, Sweden

************************************************************************/

#include "StdAfx.h"
#include "csvparser.h"
#include "meos_util.h"
#include "meosexception.h"

using namespace std;

csvparser::csvparser() {
  LineNumber=0;
}

csvparser::~csvparser() = default;

csvparser::CSV csvparser::iscsv(const wstring &file) {
  ifstream fin(path2str(file).c_str(), ios::binary);

  if (!fin.good())
    return CSV::NoCSV;

  char bf[2048];
  bool isCSVType = false;
  while (fin.good() && !isCSVType) {
    fin.getline(bf, 2048);
    isCSVType = strlen(bf) >= 3;
  }

  fin.close();

  if (!isCSVType)
    return CSV::NoCSV;

  vector<char *> sp;
  split(bf, sp);
  if (sp.size() > 0) {
    string sp0 = sp[0];
    if (sp0.find("<?xml") != string::npos)
      return CSV::NoCSV;
  }

  if (sp.size()==1 && strcmp(sp[0], "RAIDDATA")==0)
    return CSV::RAID;

  if (sp.size()<2)//No csv
    return CSV::NoCSV;

  if (strcasecmp(sp[1], "Descr")==0 || strcasecmp(sp[1], "Namn")==0
      || strcasecmp(sp[1], "Descr.")==0 || strcasecmp(sp[1], "Navn")==0)
    return CSV::OS;
  else return CSV::OE;
}

bool csvparser::openOutput(const wstring &filename, bool writeUTF)
{
  checkWriteAccess(filename);
  fout.open(path2str(filename).c_str(), ios::binary);

  if (fout.bad())
    return false;

  if (writeUTF) {
    fout.put((char)-17);
    fout.put((char)-69);
    fout.put((char)-65);
  }
  return true;
}

bool csvparser::outputRow(const string &row)
{
  fout << row << endl;
  return true;
}

bool csvparser::outputRow(const vector<wstring>& out) {
  vector<string> outUTF(out.size());
  for (size_t i = 0; i < out.size(); i++)
    outUTF[i] = toUTF8(out[i]);

  return outputRow(outUTF);
}

bool csvparser::outputRow(const vector<string> &out) {
  int size=out.size();

  for(int i=0;i<size;i++){
    string p=out[i];

    //Replace " with '
    size_t found=p.find_first_of("\"");
    while (found!=string::npos){
      p[found]='\'';
      found=p.find_first_of("\"",found+1);
    }

    if (i>0) fout << ";";

    if (p.find_first_of("; ,\t.")!=string::npos)
      fout << "\"" << p << "\"";
    else fout << p;
  }
  fout << endl;
  fout.flush();
  return true;
}

bool csvparser::closeOutput()
{
  fout.close();
  return true;
}

int csvparser::split(char *line, vector<char *> &split_vector)
{
  split_vector.clear();
  int len=(int)strlen(line);
  bool cite=false;

  for(int m=0;m<len;m++)
  {
    char *ptr=&line[m];

    if (*ptr=='"')
      ptr++;

    while(line[m])
    {
      if (!cite && line[m]==';')
        line[m]=0;
      else
      {
        if (line[m]=='"')
        {
          cite=!cite;
          line[m]=0;
          if (cite)
            ptr=&line[m+1];
        }
        m++;
      }
    }
    line[m]=0;

    split_vector.push_back(ptr);
  }

  return 0;
}

int csvparser::split(wchar_t *line, vector<wchar_t *> &split_vector)
{
  split_vector.clear();
  int len=(int)wcslen(line);
  bool cite=false;

  for(int m=0;m<len;m++)
  {
    wchar_t *ptr=&line[m];

    if (*ptr=='"')
      ptr++;

    while(line[m])
    {
      if (!cite && line[m]==';')
        line[m]=0;
      else
      {
        if (line[m]=='"')
        {
          cite=!cite;
          line[m]=0;
          if (cite)
            ptr=&line[m+1];
        }
        m++;
      }
    }
    line[m]=0;

    split_vector.push_back(ptr);
  }

  return 0;
}

void csvparser::parseUnicode(const wstring &file, list< vector<wstring> > &data) {
  fin.open(path2str(file).c_str(), ios::in | ios::binary);
  if (!fin.is_open()) return;
  fin.seekg(0, ios_base::end);
  int len = (int)fin.tellg()-2;
  if (len <= 0) {
    fin.close();
    return;
  }
  fin.seekg(2); // BOM
  vector<wchar_t> bf(len / 2 + 1);
  fin.read((char *)&bf[0], len);
  fin.close();

  int spp = 0;
  for (int k = 0; k < len / 2; k++) {
    if (bf[spp] == '\r')
      spp++;
    if (bf[k] == '\n') {
      bf[k] = 0;
      if (k > 0 && bf[k - 1] == '\r')
        bf[k - 1] = 0;
      wchar_t *linePtr = &bf[spp];
      vector<wchar_t *> sp;
      split(linePtr, sp);
      if (!sp.empty()) {
          vector<wstring> row;
          for (auto ptr : sp) row.push_back(ptr);
          data.push_back(row);
      }
      spp = k + 1;
    }
  }
  if (spp + 1 < len / 2) {
    wchar_t *linePtr = &bf[spp];
    vector<wchar_t *> sp;
    split(linePtr, sp);
    if (!sp.empty()) {
        vector<wstring> row;
        for (auto ptr : sp) row.push_back(ptr);
        data.push_back(row);
    }
  }
}

void csvparser::parse(const wstring &file, list<vector<wstring>> &data) {
  data.clear();

  fin.open(path2str(file).c_str(), ios::binary);
  
  if (!fin.good())
    throw meosException(L"Failed to read file, " + file);

  bool isUTF8 = false;
  bool detectType = true;
  string rbf;
  
  // We need to read a bit to detect type
  while(std::getline(fin, rbf)) {
    if (detectType) {
      detectType = false;
      if (rbf.length() >= 3 && (uint8_t)rbf[0] == 0xEF && (uint8_t)rbf[1] == 0xBB && (uint8_t)rbf[2] == 0xBF) {
        isUTF8 = true;
        // Continue but with BF offset if we use it later
      }
      else if (rbf.length() >= 2 && (uint8_t)rbf[0] == 0xFF && (uint8_t)rbf[1] == 0xFE) {
        fin.close();
        parseUnicode(file, data);
        return;
      }
    }
    
    wstring w;
    if (isUTF8) {
        w = fromUTF8(rbf);
        if (data.empty() && rbf.length() >= 3 && (uint8_t)rbf[0] == 0xEF) {
            // strip BOM if it was just read
            w = fromUTF8(rbf.substr(3));
        }
    }
    else {
      w = recodeToWide(rbf);
    }
    
    vector<wchar_t *> sp;
    vector<wchar_t> wline(w.begin(), w.end());
    wline.push_back(0);
    split(&wline[0], sp);
    
    if (!sp.empty()) {
      vector<wstring> row;
      for (auto ptr : sp) row.push_back(ptr);
      data.push_back(row);
    }
  }

  fin.close();
}

void csvparser::convertUTF(const wstring &file) {
  ifstream fin(path2str(file).c_str(), ios::binary);
  string rbf;

  if (!fin.good())
    throw meosException("Failed to read file");

  bool firstLine = true;
  vector<string> out;
  while (std::getline(fin, rbf)) {
    if (firstLine) {
      firstLine = false;
      if (rbf.length() >= 3 && (uint8_t)rbf[0] == 0xEF && (uint8_t)rbf[1] == 0xBB && (uint8_t)rbf[2] == 0xBF) {
        fin.close();
        return; // Already UTF-8 with BOM
      }
    }
    wstring w = recodeToWide(rbf);
    out.push_back(toUTF8(w));
  }
  
  fin.close();
  
  ofstream fout(path2str(file).c_str(), ios::binary);
  fout.put((char)0xEF);
  fout.put((char)0xBB);
  fout.put((char)0xBF);
  for (string &line : out)
    fout << line << endl;
}
