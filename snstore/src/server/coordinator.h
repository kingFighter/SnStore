#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <RCFProto.hpp>
#include "../RCFProto/snstore.pb.h"
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

#include <google/protobuf/text_format.h>

#include "worker.hpp"
#include "requestQueue.hpp"
#include "transaction.hpp"

typedef boost::shared_ptr<Transaction> TransactionPtr;
typedef boost::shared_ptr<Request> RequestPtr;

using namespace google::protobuf;

class Coordinator : public DbService{
public:
  Coordinator(int workerNum, int down, int up);
  ~Coordinator();
  void get(RpcController* controller,GetRequest* request,GetResponse* response,Closure* done);
  void put(RpcController* controller,PutRequest* request,PutResponse* response,Closure* done);
  void getrange(RpcController* controller,GRRequest* request,GRResponse* response,Closure* done);
  void execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done);

private:
  vector<Worker*> workers;
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
};
#endif
