#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm> 
#ifdef _WIN32
#include <windows.h> 
#endif
#include "snstore.h"


using namespace std;
using namespace Kevin;

string genRandomString(int);
template <class T>
bool expect(const T&, const T&);
void colorOut(int, int, const string&);
void failRed(const string&);
void passGreen(const string&);

int main() {
  vector<pair<int, string> > dbKV;
  vector<int> dbKey;
  vector<string> dbValue;
  const int NUM = 10;
  /*  srand(time(NULL));*/
  cout << "Generate Random db key and values.\n";
  for (int i = 0; i < NUM; ++i) {
    dbKey.push_back(rand());
    int len = rand() % 10 + 1;
    dbValue.push_back(genRandomString(len));
    dbKV.push_back(make_pair(dbKey[i], dbValue[i]));
  }

  SnStore db; 
  vector<pair<int, string> >::const_iterator it;
  for (it = dbKV.begin(); it != dbKV.end(); it++)
    db.put(it->first, it->second);

  bool allPassed = true;
  cout << "Test put&get functions: \n";
  bool passed = true;
  for (it = dbKV.begin(); it != dbKV.end(); it++) {
    string v = db.get(it->first);
    if (!expect(it->second, v))
      passed = false;
  }
  if (passed) {
    passGreen("Test put&get functions passed.\n");
  } else {
    failRed("Test put&get functions failed.\n");
    allPassed = false;
  }
  
  cout << "Test getRange function: \n";
  passed = true;
  int minKey = *min_element(dbKey.begin(),dbKey.end());
  int maxKey = *max_element(dbKey.begin(),dbKey.end());
  vector<string> rangeValue = db.getRange(minKey, maxKey);
  sort(rangeValue.begin(), rangeValue.end());
  vector<string> dbValueCopy = dbValue;
  sort(dbValueCopy.begin(), dbValueCopy.end());
  if (rangeValue == dbValueCopy) {
    passGreen("Test getRange function passed.\n");
  } else {
    failRed("Test getRange function failed.\n");
    allPassed = false;
  }

  if (allPassed) {
    passGreen("Test All passed.\n");
  } else {
    failRed("Test All failed.\n");
  }
  return 0;
}

template <class T>
bool expect(const T& expected, const T& realV) {
  cout << "Excepted realV: " << expected << endl;
  cout << "Your realV: " << realV << endl;
  if (expected == realV) {
    passGreen("Matched.\n");
    return true;
  } else {
    failRed("Not Matched.\n");
    return false;
  }
}

string genRandomString(const int len) {
  static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  
  string str;
  for (int i = 0; i < len; ++i) 
    str.push_back(alphanum[rand() % (sizeof(alphanum) - 1)]);
  
  return str;
}
void colorOut(int winColor, int otherColor, const string& msg) {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
  HANDLE hConsole;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  // save original color
  GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
  SetConsoleTextAttribute(hConsole, winColor);
  cout << msg;
  // reset
  SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
#elif 
  cout << "\033[1;" << otherColor << "m" << msg << "\033[0m\n";
#endif

}
void failRed(const string& msg) {
  colorOut(12, 31, msg);
}
void passGreen(const string& msg) {
  colorOut(10, 32, msg);
}
