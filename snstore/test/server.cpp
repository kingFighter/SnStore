#include "../src/server/coordinator.h"
#include <iostream>
#include <boost/thread.hpp>
#include <sstream>

using namespace std;
RequestQueue::QueueType t;
Coordinator coor(5, 1, 100, t);//initial for test

double totalTime = 0;
int totalNum = 0;
class Test {
public:
  void start(int times) {
    mThread = boost::thread(&Test::work, this, times);
  }
  void join() {
    mThread.join();
  }
  void work(int times) {
    clock_t start, finish;
    start = clock();
    for (int i = 0; i < times; ++i) {
      coor.put(i % 100 + 1, i);
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
  Test t[4];
  for (int i = 0; i < 4; ++i) {
    t[i].start(atoi(argv[1]));
    t[i].join();
  }
  cout << totalNum / totalTime << endl;
}
