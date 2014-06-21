#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <map>
#include <sstream>
#include <boost/thread.hpp>

#ifdef _WIN32
#include <windows.h>
#endif
#include "../src/client/snstore.h"

using namespace std;
using namespace Kevin;


class Tran {
public:
  void work(int n);
  void start(int n);
  void join();
  bool getPass() const {return pass;}
private:
  boost::thread mThread;
  bool pass;
};

string genRandomString(int);
template <class T>
bool expect(const T&, const T&);
void colorOut(int, int, const string&);
void failRed(const string&);
void passGreen(const string&);
void testCorrectness();
void testPerformance();
string number2String (int num);
bool testTransaction();
void testThroughput();

int main() {
  // const int TRAN_NUM = 200;
  // for (int i = 0; i < TRAN_NUM; ++i)
  //   testThroughput();
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
    int key = rand() % KEY_LIMIT + 1;
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
  int size = maxKey - minKey + 1;
  cout << rangeValue.size() << " " << size << endl;
  for (int i = 0; i < size; ++i) {
    // cerr << rangeValue[i] << " " << dbKV[i + minKey] << endl;
    if (rangeValue[i] != dbKV[i + minKey]) {
      cout << "Expected: " << dbKV[i + minKey] << endl
           << "Real: " << rangeValue[i] << endl;
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
  int key = rand() % KEY_LIMIT + 1;
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
  for (int i = testKeyMin; i <= testKeyMax; ++i) {
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

  passed = true;
  const int TIMES = 100;
  for (int i = 0; i < TIMES; ++i) {
    if (!testTransaction()) {
      passed = false;
      break;
    }
  }

  if (passed) {
    passGreen("Test mul transaction commit successful passed.\n");
  } else {
    failRed("Test mul transaction commit successful failed.\n");
    allPassed = false;
  }
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
    dbPer.get(rand() % KEY_LIMIT + 1);
    int len = rand() % VALUE_LEN_LIMIT + 1;
    string value = genRandomString(len);
    dbPer.put(rand() % KEY_LIMIT + 1, value);
    int minKey = rand() % KEY_LIMIT + 1;
    int maxKey = rand() % KEY_LIMIT + 1;
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

string number2String (int num) {
   ostringstream ss;
   ss << num;
   return ss.str();
}


void Tran::work(int n) {
  SnStore db;
  string str = "thread" + number2String(n);
  db.beginTx();
  db.put(1, str);
  db.put(2, str);
  db.get(1);
  db.get(2);
  map<int, string> results = db.commit();
  if (results[1] == str && results[2] == str) {
    passGreen(str + " passed\n");
    pass = true;
  } else {
    cout << "Excepted: " << str << endl;
    cout << "Real: " << results[1] << ", " << results[2] << endl;
    failRed(str + " failed.\n");
    pass = false;
  }
}
void Tran::start(int n) {
    mThread = boost::thread(&Tran::work, this, n);
}

void Tran::join() {
    mThread.join();
}

bool testTransaction() {
   const int THREAD_NUM = 3;
   Tran trs[THREAD_NUM];
   bool re = true;
   for (int i = 0; i < THREAD_NUM; ++i) {
     trs[i].start(i);
     trs[i].join();
   }
   
   for (int i = 0; i < THREAD_NUM; ++i) {
     if (!trs[i].getPass()) {
       re = false;
       break;
     }
   }
   
   return re;
}

void testThroughput() {
  SnStore db;
  const int KEY_LIMIT = 100;
  const int VALUE_LEN_LIMIT = 10;
  int key1 = rand() % KEY_LIMIT + 1;
  int key2 = rand() % KEY_LIMIT + 1;
  if (key1 > key2)
    swap(key1, key2);
  int len = rand() % VALUE_LEN_LIMIT + 1;
  string value = genRandomString(len);
  db.beginTx();
  db.get(key1);
  db.put(key1, value);
  db.getRange(key1, key2);
  db.commit();
}
