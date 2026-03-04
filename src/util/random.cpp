#include "random.h"
#include "win_types.h"
#include <vector>
#include <algorithm>

using namespace std;

unsigned short _rbsource[256];

static int r_index = 0;
static int r_bit = 0;

void InitRandom(int seed)
{
  for (int i=0; i<256; i++)
    _rbsource[i] = (unsigned short)(rand());
}

bool GetRandomBit()
{
  bool res = (_rbsource[r_index] & (1<<r_bit)) != 0;
  r_bit++;
  if (r_bit >= 16) {
    r_bit = 0;
    r_index++;
    if (r_index >= 256) r_index = 0;
  }
  return res;
}

int GetRandomNumber(int m)
{
  if (m <= 1) return 0;
  DWORD r=0;
  for (int i=0; i<31; i++) {
    if (GetRandomBit())    r|=(1<<i);
  }
  return r%m;
}

void permute(vector<int> &vec)
{
  for (size_t i=0; i<vec.size(); i++) {
    int j = GetRandomNumber(vec.size());
    swap(vec[i], vec[j]);
  }
}
