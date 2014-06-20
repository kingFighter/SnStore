#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <map>

#ifdef _WIN32
#include <windows.h>
#endif
#include "../src/client/snstore.h"


using namespace std;
using namespace Kevin;

string genRandomString(int);
template <class T>
bool expect(const T&, const T&);
void colorOut(int, int, const string&);
void failRed(const string&);
void passGreen(const string&);
void testCorrectness();
void testPerformance();

int main() {
  // testCorrectness();
  // testPerformance();
  SnStore db;
  int key = 21;
  string value = "1";
  map<int, string> results;
  db.put(21, "1");
  db.put(22, "1");
  db.put(32, "1");
  db.put(93, "2");
  db.beginTx();
  db.getRange(21, 93);
  results = db.commit();
  // end commit
  vector<string> r = db.getRange(21, 93);
  for (int i = 21; i <= 93; i++) {
    if (r[i - 21] != results[i]) {
      cout << r[i - 21] << " " << results[i] << endl;
    }
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

void testCorrectness() {
  map<int, string> dbKV;
  const int NUM = 10;
  const int SENUM = 20;
  const int KEY_LIMIT = 100;
  const int VALUE_LEN_LIMIT = 10;
  /*  srand(time(NULL));*/
  cout << "Generate Random db key and values.\n";
  for (int i = 0; i < NUM; ++i) {
    int key = rand() % KEY_LIMIT;
    int len = rand() % VALUE_LEN_LIMIT + 1;
    string value = genRandomString(len);
    dbKV.insert(make_pair(key, value));
  }
  cout << string(SENUM, '*') << endl << endl;

  SnStore db;
  map<int, string>::const_iterator it;
  set<int> dbKey;
  for (it = dbKV.begin(); it != dbKV.end(); it++) {
    db.put(it->first, it->second);
    dbKey.insert(it->first);
  }
  cout << "SnStore is setup.\n ";

  bool allPassed = true;
  cout << string(SENUM, '*') << endl;
  cout << "Test Correctness.\n";
  cout << string(SENUM, '-') << endl;

  // Test put&get
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
  cout << string(SENUM, '-') << endl << endl;
  // End of put&get

  // Test getRange
  cout << string(SENUM, '-') << endl;
  cout << "Test getRange function: \n";
  passed = true;
  int minKey = *min_element(dbKey.begin(),dbKey.end());
  int maxKey = *max_element(dbKey.begin(),dbKey.end());
  vector<string> rangeValue = db.getRange(minKey, maxKey);
  int size = maxKey - minKey;
  // cerr << dbKV.size() << " " << size << endl;
  for (int i = 0; i <= size; ++i) {
    // cerr << rangeValue[i] << " " << dbKV[i + minKey] << endl;
    if (rangeValue[i] != dbKV[i + minKey]) {
      passed = false;
      break;
    }
  }

  if (passed) {
    passGreen("Test getRange function passed.\n");
  } else {
    failRed("Test getRange function failed.\n");
    allPassed = false;
  }
  cout << string(SENUM, '-') << endl << endl;
  // End getRange

  // Test transaction
  cout << string(SENUM, '-') << endl;
  cout << "Test transaction: \n";
  passed = true;
  cout << string(SENUM, '+') << endl;
  cout << "Test transaction commit sucessful.\n";
  // transaction commit successful test
  int key = rand() % KEY_LIMIT;
  string v = db.get(key);
  // We can replace the testKeyMin, testKeyMax if needed
  int testKeyMin = minKey;
  int testKeyMax = maxKey;
  
  // begin tx
  int len = rand() % VALUE_LEN_LIMIT + 1;
  string value = genRandomString(len);
  map<int, string> results;
  db.beginTx();
  db.get(key);
  db.put(key, value);
  db.put(key + 2, value);
  db.get(key);
  db.get(key + 2);
  db.getRange(testKeyMin, testKeyMax);
  results = db.commit();
  // end commit
  rangeValue = db.getRange(testKeyMin, testKeyMax);
  v = db.get(key);
  if (!expect(v, results[key])) {
    failRed("Test get/put failed\n");
    passed = false;
  } else {
    passGreen("Test get/put passed\n");
  }

  v = db.get(key + 2);
  if (!expect(v, results[key + 2])) {
    failRed("Test get/put2 failed\n");
    passed = false;
  } else {
    passGreen("Test get/put2 passed\n");
  }
  
  db.beginTx();
  db.getRange(testKeyMin, testKeyMax);
  results = db.commit();
  rangeValue = db.getRange(testKeyMin, testKeyMax);
  cout << testKeyMin << " " << testKeyMax << endl;
  for (int i = testKeyMin; i <= testKeyMax; ++i) {
    // cout << "i : " << i << endl;
    // cout << "Expected: " << rangeValue[i - testKeyMin] << endl;
    // cout << "Results: " << results[i] << endl;
    if (rangeValue[i - testKeyMin] != results[i]) {
      cout << "i : " << i << endl;
      cout << "Expected: " << rangeValue[i - testKeyMin] << endl;
      cout << "Results: " << results[i] << endl;
      
      passed = false;
      break;
    }
  }

  cout << string(SENUM, '+') << endl;
  if (passed) {
    passGreen("Test transaction commit successful passed.\n");
  } else {
    failRed("Test transaction commit successful failed.\n");
    allPassed = false;
  }
  cout << string(SENUM, '-') << endl << endl;
  // End transaction

  if (allPassed) {
    passGreen("Test All passed.\n");
  } else {
    failRed("Test All failed.\n");
  }
  cout << string(SENUM, '*') << endl << endl;

}
void testPerformance() {
  // Performance Test
  SnStore dbPer;
  clock_t start, finish;
  const int SENUM = 20;
  const int KEY_LIMIT = 100;
  const int VALUE_LEN_LIMIT = 10;

  double totalTime;
  cout << string(SENUM, '*') << endl;
  cout << "Test Performance.\n";
  cout << string(SENUM, '-') << endl;
  const int TEST_NUM = 100;
  cout << "Test put performance with " << TEST_NUM << " :\n";
  start = clock();
  for (int i = 0; i < TEST_NUM; ++i) {
    string value = genRandomString(rand() % 20 + 1);
    dbPer.put(i, value);
  }
  finish = clock();
  totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
  cout << "Time: " << totalTime << "s\n";
  cout << string(SENUM, '-') << endl << endl;

  cout << string(SENUM, '-') << endl;
  cout << "Test get performance with " << TEST_NUM << " :\n";
  start = clock();
  for (int i = 0; i < TEST_NUM; ++i) {
    dbPer.get(i);
  }
  finish = clock();
  totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
  cout << "Time: " << totalTime << "s\n";
  cout << string(SENUM, '-') << endl << endl;

  cout << string(SENUM, '-') << endl;
  cout << "Test getRange performance with " << TEST_NUM << " :\n";
  start = clock();
  dbPer.getRange(0, TEST_NUM - 1);
  finish = clock();
  totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
  cout << "Time: " << totalTime << "s\n";
  cout << string(SENUM, '-') << endl << endl;

  cout << string(SENUM, '-') << endl;
  cout << "Test transaction performance with " << TEST_NUM << " :\n";
  start = clock();
  dbPer.beginTx();
  for (int i = 0; i < TEST_NUM; ++i) {
    dbPer.get(rand() % KEY_LIMIT);
    int len = rand() % VALUE_LEN_LIMIT + 1;
    string value = genRandomString(len);
    dbPer.put(rand() % KEY_LIMIT, value);
    int minKey = rand() % KEY_LIMIT;
    int maxKey = rand() % KEY_LIMIT;
    if (minKey > maxKey)
      swap(minKey, maxKey);
    dbPer.getRange(minKey, maxKey);
  }
  dbPer.commit();

  finish = clock();
  totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
  cout << "Time: " << totalTime << "s\n";
  cout << string(SENUM, '-') << endl << endl;

}
