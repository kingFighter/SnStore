#include "../src/server/coordinator.h"
#include <iostream>
#include <boost/thread.hpp>
#include <sstream>
#include <map>

using namespace std;
RequestQueue::QueueType t;
Coordinator coor(2, 1, 40, t);//initial for test

double totalTime = 0;
int totalNum = 0;
class Test {
public:
  void start(int times) {
    mThread = boost::thread(&Test::workGet, this, times);
  }
  void join() {
    mThread.join();
  }

  void workGet(int times) {
    clock_t start, finish;
    int num[2] ={1, 21};
    map<int, int> m;
    start = clock();
    for (int i = 0; i < times; ++i) {
      coor.get(i % 40 + 1);
    }
    finish = clock();
    totalTime += (double)(finish - start) / CLOCKS_PER_SEC;
    totalNum += times;
  }
  
  void workPut(int times) {
    clock_t start, finish;
    int num[2] ={1, 21};
    map<int, int> m;
    start = clock();
    for (int i = 0; i < times; ++i) {
      coor.put(i % 40 + 1, i % 40 + 1);
    }
    finish = clock();
    totalTime += (double)(finish - start) / CLOCKS_PER_SEC;
    totalNum += times;
  }
    

  void work2(int times) {
    clock_t start, finish;
    int num[2] ={1, 21};
    map<int, int> m;
    start = clock();
    for (int i = 0; i < times / 5; ++i) {
      coor.getRange(num[i % 2], num[i % 2] + 2, m);
      coor.getRange(num[i % 2], num[i % 2] + 2, m);
      coor.getRange(num[i % 2], num[i % 2] + 2, m);
      coor.getRange(num[i % 2], num[i % 2] + 2, m);
      coor.getRange(1, 30, m);
      // coor.get(i % 100 + 1);
    }
    finish = clock();
    totalTime += (double)(finish - start) / CLOCKS_PER_SEC;
    totalNum += times;
  }
  
  void work(int times) {
    clock_t start, finish;
    int num[2] ={1, 21};
    map<int, int> m;
    start = clock();
    for (int i = 0; i < times / 5; ++i) {
      coor.getRange(num[i % 2], num[i % 2] + 2, m);
      coor.getRange(1, 30, m);
      coor.getRange(1, 30, m);
      coor.getRange(1, 30, m);
      coor.getRange(1, 30, m);
      // coor.get(i % 100 + 1);
    }
    finish = clock();
    totalTime += (double)(finish - start) / CLOCKS_PER_SEC;
    totalNum += times;
  }
private:
  boost::thread mThread;
};
string number2String (int num) {
  ostringstream ss;
  ss << num;
  return ss.str();
}

int main(int argc,char *argv[]) {
  const int NUM = 1;
  Test t[NUM];
  int d[10] = {10, 100, 500, 1000, 5000, 10000, 15000, 20000, 30000, 50000};
  for (int j = 0; j < 10; ++j) {
    totalNum = 0;
    totalTime = 0;
    for (int i = 0; i < NUM; ++i) {
      // t[i].start(atoi(argv[1]));
      t[i].start(d[j]);
      t[i].join();
    }
    cout << totalNum / totalTime << endl;
  }
}
