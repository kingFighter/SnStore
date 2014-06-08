#ifndef WORKER_H
#define WORKER_H
#include <queue>
#include <boost/thread.hpp>

using namespace std;

class Worker {
public:
  Worker(){}
  Worker(const Worker &worker) {
  }
  Worker& operator=(const Worker &worker){
    return *this;
  }
  void set(queue<string> *operations_, queue<string> *results_, boost::condition_variable *c_);
  void start();
  void processRequests();
  ~Worker(){}
  
 private:
  boost::thread processThread;
  boost::mutex worker_mutex;
  /* Below is from server */
  boost::condition_variable *c;
  queue<string> *operations, *results;
};

#endif
