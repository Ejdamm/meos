#include "csvparser.h"
#include "meos_util.h"
#include "meosexception.h"
#include "localizer.h"
#include "xmlparser.h"
#include <algorithm>
#include <cstring>
#include <cwchar>
#include <set>
#include <cassert>

using namespace std;

class CSVLineWrapper {
  const vector<wstring> &data;
  int row;
public:
  CSVLineWrapper(int row, const vector<wstring>& data) : data(data), row(row) {
  }

  const wstring& operator[](int i) const {
    if (i < 0 || (size_t)i >= data.size()) {
      throw meosException("Invalid CSV file. Incorrect data specification on line X" + itos(row));
    }
    return data[i];
  }

  size_t size() const { return data.size(); }
};

csvparser::csvparser() {
  LineNumber=0;
  nimport = 0;
}

csvparser::~csvparser() = default;

csvparser::CSV csvparser::iscsv(const wstring &file) {
  string sfile;
  wide2String(file, sfile);
  ifstream fin(sfile);

  if (!fin.good())
    return CSV::NoCSV;

  char bf[2048];
  bool isCSVType = false;
  while (fin.good() && !isCSVType) {
    fin.getline(bf, 2048);
    isCSVType = strlen(bf) >= 3;
  }

  fin.close();

  vector<char *> sp_vec;
  split(bf, sp_vec);
  if (sp_vec.size() > 0) {
    string sp0 = sp_vec[0];
    if (sp0.find("<?xml") != string::npos)
      return CSV::NoCSV;
  }

  if (sp_vec.size()==1 && strcmp(sp_vec[0], "RAIDDATA")==0)
    return CSV::RAID;

  if (sp_vec.size()<2)//No csv
    return CSV::NoCSV;

  if (strcasecmp(sp_vec[1], "Descr")==0 || strcasecmp(sp_vec[1], "Namn")==0
      || strcasecmp(sp_vec[1], "Descr.")==0 || strcasecmp(sp_vec[1], "Navn")==0)
    return CSV::OS;
  else return CSV::OE;
}

bool csvparser::openOutput(const wstring &filename, bool writeUTF)
{
  checkWriteAccess(filename);
  string sfile;
  wide2String(filename, sfile);
  fout.open(sfile);

  if (fout.bad())
    return false;

  if (writeUTF) {
    fout.put((char)0xEF);
    fout.put((char)0xBB);
    fout.put((char)0xBF);
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
  for (size_t i = 0; i < out.size(); i++) {
      wide2String(out[i], outUTF[i]); // Simplification
  }
  return outputRow(outUTF);
}

bool csvparser::outputRow(const vector<string> &out) {
  int size=out.size();

  for(int i=0;i<size;i++){
    string p=out[i];

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
  int len=strlen(line);
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
  int len=wcslen(line);
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

void csvparser::parse(const wstring &file, list<vector<wstring>> &data) {
  data.clear();
  string sfile;
  wide2String(file, sfile);
  fin.open(sfile);
  
  if (!fin.good())
    throw meosException(L"Failed to read file: " + file);

  string rbf;
  vector<wchar_t> wbf_a;
  fin.seekg(0, ios::end);
  size_t flen = fin.tellg();
  fin.seekg(0);
  wbf_a.resize(flen + 1);
  wchar_t *wbf = &wbf_a[0];

  bool isUTF8 = false;
  bool firstLine = true;
  while(std::getline(fin, rbf)) {
    const char *bf = rbf.c_str();
    if (firstLine) {
      firstLine = false;
      if (rbf.length() >= 3 && uint8_t(rbf[0]) == 0xEF && uint8_t(rbf[1]) == 0xBB && uint8_t(rbf[2]) == 0xBF) {
        isUTF8 = true;
        bf += 3;
      }
    }
   
    wstring w;
    string2Wide(bf, w);
    wcscpy(wbf, w.c_str());
    
    vector<wchar_t *> sp_vec;
    split(wbf, sp_vec);
    
    if (!sp_vec.empty()) {
      data.push_back(vector<wstring>());
      data.back().resize(sp_vec.size());
      for (size_t k = 0; k < sp_vec.size(); k++) {
        data.back()[k] = sp_vec[k];
      }
    }
  }
  fin.close();
}

// Stubs for domain-dependent methods
bool csvparser::importOS_CSV(oEvent &oe, const wstring &file) { return false; }
bool csvparser::importOE_CSV(oEvent &oe, const wstring &file) { return false; }
bool csvparser::importOCAD_CSV(oEvent &oe, const wstring &file, bool addClasses) { return false; }
bool csvparser::importRAID(oEvent &oe, const wstring &file) { return false; }
bool csvparser::importPunches(const oEvent &oe, const wstring &file, vector<PunchInfo> &punches) { return false; }
bool csvparser::importCards(const oEvent &oe, const wstring &file, vector<SICard> &cards) { return false; }
int csvparser::importRanking(oEvent &oe, const wstring &file, vector<wstring> & problems) { return 0; }
void csvparser::importTeamLineup(const wstring &file, const map<wstring, int> &classNameToNumber, vector<TeamLineup> &teams) {}
void csvparser::convertUTF(const wstring &file) {}
bool csvparser::checkSimanLine(const oEvent &oe, const CSVLineWrapper &sp, SICard &card) { return false; }
void csvparser::checkSIConfigHeader(const CSVLineWrapper &sp) {}
bool csvparser::checkSIConfigLine(const oEvent &oe, const CSVLineWrapper &sp, SICard &card) { return false; }
const wchar_t *csvparser::getSIC(SIConfigFields sic, const CSVLineWrapper&sp) const { return L""; }
void csvparser::parseUnicode(const wstring &file, list<vector<wstring>> &data) {}
int csvparser::selectPunchIndex(const wstring &competitionDate, const CSVLineWrapper &sp, int &cardIndex, int &timeIndex, int &dateIndex, wstring &processedTime, wstring &date) { return 0; }
