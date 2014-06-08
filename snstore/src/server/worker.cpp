#include "worker.h"

void Worker::start() {
  processThread = boost::thread(&Worker::processRequests, this);
}
void Worker::set(queue<string> *operations_, queue<string> *results_, 
                 boost::condition_variable *c_) {
  operations = operations_;
  results = results_;
  c = c_;
}

void Worker::processRequests() {
  
}
