#ifndef WORKER_H
#define WORKER_H
#include <queue>
#include <boost/thread.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "../utility/utility.h"

using namespace std;

class Worker {
public:
  Worker(){}
  Worker(const Worker &worker) {
  }
  Worker& operator=(const Worker &worker){
    return *this;
  }
  void set(queue<string> *operations_, queue<string> *results_, 
           boost::condition_variable *operations_con_, 
           boost::condition_variable *results_con_);
  void start();
  void processRequests();
  ~Worker(){}
  
 private:
  map<int, string> db;
  boost::thread processThread;
  boost::mutex worker_mutex;
  /* Below is from server */
  boost::condition_variable *operations_con, *results_con;
  queue<string> *operations, *results;
};

#endif
