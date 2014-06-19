#include "coordinator.h"

// For simplicity, Coordinator is responsible for  [down, up]
// work[0]: [down, down + size - 1]
// work[1]: [down + size, down + size * 2 - 1];
// ...
// work[workerNum - 1]: [up - size - 1, up]
Coordinator::Coordinator(int workerNum_, int down_, int up_) : workerNum(workerNum_), down(down_), up(up_) {
  size = (up - down + 1) / workerNum;
  for (int i = 0; i < workerNum; ++i) {
    workers.push_back(new Worker(down + i * size, down + (i + 1) * size - 1, RequestQueue::WAIT_FREE));
  }
}

Coordinator::~Coordinator()
{
}

void
Coordinator::get(RpcController* controller,GetRequest* request,GetResponse* response,Closure* done)
{
  int32 key = request->key();
  TransactionPtr tx = TransactionPtr(new Transaction());
  RequestPtr r = RequestPtr(new Request(tx));
  r -> pushOp(Request::createGetOp(key));
  workers[pos(key)]->pushRequest(r);
  tx->wait();
  std::map<int, std::string> m = tx -> getResults();
  response->set_value(m[0]);
  done->Run();
}

void
Coordinator::put(RpcController* controller,PutRequest* request,PutResponse* response,Closure* done)
{
  int32 key = request->key();
  string value = request->value();
  TransactionPtr tx = TransactionPtr(new Transaction());
  RequestPtr r = RequestPtr(new Request(tx));
  r -> pushOp(Request::createPutOp(key, value));
  workers[pos(key)]->pushRequest(r);
  tx->wait();
  std::map<int, std::string> m = tx -> getResults();
  response->set_result(true);
  done->Run();
}

void
Coordinator::getrange(RpcController* controller,GRRequest* request,GRResponse* response,Closure* done)
{

}


void
Coordinator::execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done)
{
}
