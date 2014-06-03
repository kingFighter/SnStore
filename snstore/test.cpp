#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>

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
    SnStore db;
    db.put(1,"2");
    db.put(2,"3");
    db.put(3,"4");
    db.put(4,"5");
    cout<<db.get(1);
    db.getRange(1,3);
//  vector<pair<int, string> > dbKV;
//  vector<int> dbKey;
//  vector<string> dbValue;
//  const int NUM = 10;
//  const int SENUM = 20;
//  /*  srand(time(NULL));*/
//  cout << "Generate Random db key and values.\n";
//  for (int i = 0; i < NUM; ++i) {
//    dbKey.push_back(rand());
//    int len = rand() % 10 + 1;
//    dbValue.push_back(genRandomString(len));
//    dbKV.push_back(make_pair(dbKey[i], dbValue[i]));
//  }
//  cout << string(SENUM, '*') << endl << endl;
//
//  SnStore db;
//  vector<pair<int, string> >::const_iterator it;
//  for (it = dbKV.begin(); it != dbKV.end(); it++)
//    db.put(it->first, it->second);
//
//  bool allPassed = true;
//  cout << string(SENUM, '*') << endl;
//  cout << "Test Correctness.\n";
//  cout << string(SENUM, '-') << endl;
//  cout << "Test put&get functions: \n";
//  bool passed = true;
//  for (it = dbKV.begin(); it != dbKV.end(); it++) {
//    string v = db.get(it->first);
//    if (!expect(it->second, v))
//      passed = false;
//  }
//  if (passed) {
//    passGreen("Test put&get functions passed.\n");
//  } else {
//    failRed("Test put&get functions failed.\n");
//    allPassed = false;
//  }
//  cout << string(SENUM, '-') << endl << endl;
//
//  cout << string(SENUM, '-') << endl;
//  cout << "Test getRange function: \n";
//  passed = true;
//  int minKey = *min_element(dbKey.begin(),dbKey.end());
//  int maxKey = *max_element(dbKey.begin(),dbKey.end());
//  vector<string> rangeValue = db.getRange(minKey, maxKey);
//  sort(rangeValue.begin(), rangeValue.end());
//  vector<string> dbValueCopy = dbValue;
//  sort(dbValueCopy.begin(), dbValueCopy.end());
//  if (rangeValue == dbValueCopy) {
//    passGreen("Test getRange function passed.\n");
//  } else {
//    failRed("Test getRange function failed.\n");
//    allPassed = false;
//  }
//  cout << string(SENUM, '-') << endl << endl;
//
//  if (allPassed) {
//    passGreen("Test All passed.\n");
//  } else {
//    failRed("Test All failed.\n");
//  }
//  cout << string(SENUM, '*') << endl << endl;
//
//  // Performance Test
//  SnStore dbPer;
//  clock_t start, finish;
//  double totalTime;
//  cout << string(SENUM, '*') << endl;
//  cout << "Test Performance.\n";
//  cout << string(SENUM, '-') << endl;
//  const int TEST_NUM = 100;
//  cout << "Test put performance with " << TEST_NUM << " :\n";
//  start = clock();
//  for (int i = 0; i < TEST_NUM; ++i) {
//    string value = genRandomString(rand() % 20 + 1);
//    dbPer.put(i, value);
//  }
//  finish = clock();
//  totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
//  cout << "Time: " << totalTime << "s\n";
//  cout << string(SENUM, '-') << endl << endl;
//
//  cout << string(SENUM, '-') << endl;
//  cout << "Test get performance with " << TEST_NUM << " :\n";
//  start = clock();
//  for (int i = 0; i < TEST_NUM; ++i) {
//    dbPer.get(i);
//  }
//  finish = clock();
//  totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
//  cout << "Time: " << totalTime << "s\n";
//  cout << string(SENUM, '-') << endl << endl;
//
//  cout << string(SENUM, '-') << endl;
//  cout << "Test getRange performance with " << TEST_NUM << " :\n";
//  start = clock();
//  dbPer.getRange(0, TEST_NUM - 1);
//  finish = clock();
//  totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
//  cout << "Time: " << totalTime << "s\n";
//  cout << string(SENUM, '-') << endl << endl;

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
#else
  cout << "\033[1;" << otherColor << "m" << msg << "\033[0m\n";
#endif

}
void failRed(const string& msg) {
  colorOut(12, 31, msg);
}
void passGreen(const string& msg) {
  colorOut(10, 32, msg);
}
