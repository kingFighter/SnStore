#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <map>
#include <vector>
#include <string>
#include <pthread.h>
#include "../utility/utility.h"
 #include <boost/thread.hpp>
#include <queue>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "worker.hpp"
#include "requestQueue.hpp"
#include "transaction.hpp"
#include "../RCF/operation.hpp"

typedef boost::shared_ptr<Transaction> TransactionPtr;
typedef boost::shared_ptr<Request> RequestPtr;
using std::endl;

class Coordinator {
public:
  Coordinator(int workerNum, int down, int up, RequestQueue::QueueType qt);
  ~Coordinator();
/*
  void get(RpcController* controller,const GetRequest* request,GetResponse* response,Closure* done);
  void put(RpcController* controller,const PutRequest* request,PutResponse* response,Closure* done);
  void getrange(RpcController* controller,const GRRequest* request,GRResponse* response,Closure* done);
  void execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done);

*/
	int get(int key);
	int put(int key, int value);
	bool getRange(int begin, int end, std::map<int, int>& result);
	bool execTx(std::vector<Operation> ops, std::map<int, int>& result);
private:
  std::vector<Worker*> workers;
  /* the number of worker */
  int workerNum;
  /* down limit */
  int down;
  int up;
  int size;
  /* global_mutex used as global lock */
  boost::mutex global_mutex;
  int pos(int key) const {
    return (key - down) / size;
  }
  RequestQueue::QueueType qt;
};
#endif
