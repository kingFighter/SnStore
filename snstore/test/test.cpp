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
  testCorrectness();
  testPerformance();

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
  bool twoPassed = true;
  cout << string(SENUM, '+') << endl;
  cout << "Test transaction commit sucessful.\n";
  // transaction commit successful test
  int key = rand() % KEY_LIMIT;
  string v = db.get(key);
  SnStore anotherDb;
  // We can replace the testKeyMin, testKeyMax if needed
  int testKeyMin = minKey;
  int testKeyMax = maxKey;
  rangeValue = db.getRange(testKeyMin, testKeyMax);
  // begin tx
  db.beginTx();
  // test get() before put()
  cout << "Test get before put:\n";
  if (!expect(v, db.get(key))) {
    failRed("Test get before put failed\n");
    passed = false;
  } else {
    passGreen("Test get before put passed.\n");
  }

  // test getRange in commit
  if (db.getRange(testKeyMin, testKeyMax) != rangeValue) {
    failRed("Test getRange in commit failed\n");
    passed = false;
  } else {
    passGreen("Test getRange in commit passed\n");
  }
  int len = rand() % VALUE_LEN_LIMIT + 1;
  string value = genRandomString(len);
  db.put(key, value);
  string vv = db.get(key);
  vector<string> inCommitRange = db.getRange(testKeyMin, testKeyMax);
  // We cannot test whether put is updated by server before commit
  // because this commit get lock.
  db.commit();
  // end commit
  // test get() after put()
  v = db.get(key);
  if (!expect(v, vv)) {
    failRed("Test get after put failed\n");
    passed = false;
  } else {
    passGreen("Test get after put passed\n");
  }

  rangeValue = db.getRange(testKeyMin, testKeyMax);
  if (rangeValue != inCommitRange) {
    failRed("Test getRange after commit failed\n");
    passed = false;
  } else {
    passGreen("Test getRange after commit passed\n");
  }

  // test put after commit is saved by server
  if (!expect(vv, anotherDb.get(key))) {
    passed = false;
    failRed("Test put after commit failed\n");
  } else {
    passGreen("Test put after commit passed\n");
  }
  cout << string(SENUM, '+') << endl;
  if (passed) {
    passGreen("Test transaction commit successful passed.\n");
  } else {
    failRed("Test transaction commit successful failed.\n");
    twoPassed = false;
    allPassed = false;
  }
  cout << string(SENUM, '+') << endl << endl;

  passed = true;
  cout << string(SENUM, '+') << endl;
  cout << "Test transaction commit abort.\n";
  // transaction commit abort test


  cout << string(SENUM, '+') << endl;
  if (passed) {
    passGreen("Test transaction commit abort passed.\n");
  } else {
    failRed("Test transaction commit abort failed.\n");
    allPassed = false;
    twoPassed = false;
  }
  cout << string(SENUM, '+') << endl << endl;


  if (twoPassed) {
    passGreen("Test transaction passed.\n");
  } else {
    failRed("Test transaction failed.\n");
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
