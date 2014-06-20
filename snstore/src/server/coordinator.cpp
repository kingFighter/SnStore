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
Coordinator::get(RpcController* controller,const GetRequest* request,GetResponse* response,Closure* done)
{
  int32 key = request->key();
  TransactionPtr tx = TransactionPtr(new Transaction());
  RequestPtr r = RequestPtr(new Request(tx));
  r -> pushOp(Request::createGetOp(key));
  workers[pos(key)]->pushRequest(r);
  tx->wait();
  std::map<int, std::string> m = tx -> getResults();
  response->set_value(m[key]);
  done->Run();
}

void
Coordinator::put(RpcController* controller,const PutRequest* request,PutResponse* response,Closure* done)
{
  int32 key = request->key();
  string value = request->value();
  //	std::cout << "Put Request!\nKey:" << key << "\nValue:" << value << "\n";
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
Coordinator::getrange(RpcController* controller,const GRRequest* request,GRResponse* response,Closure* done)
{

  int32 start = request->start();
  int32 end = request->end();
  int startPos = pos(start);
  int endPos = pos(end);

  TransactionPtr tx = TransactionPtr(new Transaction());

  if (startPos == endPos) {
    RequestPtr r = RequestPtr(new Request(tx));
    r -> pushOp(Request::createGetRangeOp(start, end));
    workers[startPos]->pushRequest(r);
  } else {
    boost::mutex::scoped_lock lock(global_mutex);
    RequestPtr r = RequestPtr(new Request(tx));
    r -> pushOp(Request::createGetRangeOp(start, (startPos + 1) * size + down - 1));
    workers[startPos]->pushRequest(r);

    for (int i = startPos + 1; i < endPos; ++i) {
      RequestPtr r = RequestPtr(new Request(tx));
      r -> pushOp(Request::createGetRangeOp(down + i * size, down + (i + 1) * size - 1));
      workers[i]->pushRequest(r);
    }

    RequestPtr r2 = RequestPtr(new Request(tx));
    r2 -> pushOp(Request::createGetRangeOp(down + endPos * size, end));
    workers[endPos]->pushRequest(r2);
  }

  tx->wait();
  std::map<int, std::string> m = tx -> getResults();
  std::map<int, std::string>::iterator it = m.begin();
  for (; it != m.end(); ++it) {
    response->add_value(it->second);
    response->set_result(true);
  }
  done->Run();
}


void
Coordinator::execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done)
{
  RepeatedPtrField<TxRequest_Request> reqs = request->reqs();
  RepeatedPtrField<TxRequest_Request>::iterator it = reqs.begin();
  boost::mutex::scoped_lock lock(global_mutex);
  TransactionPtr tx = TransactionPtr(new Transaction());
  for(;it != reqs.end(); it++) {
    switch (it->op()) {
    case TxRequest_Request::GET: {
      cout << "GET" << endl;
      int32 key = it->key1();
      RequestPtr r = RequestPtr(new Request(tx));
      r -> pushOp(Request::createGetOp(key));
      workers[pos(key)]->pushRequest(r);
      break;
    }
    case TxRequest_Request::PUT: {
      cout << "PUT" << endl;
      int32 key = it->key1();
      string value = it->value();
      RequestPtr r = RequestPtr(new Request(tx));
      r -> pushOp(Request::createPutOp(key, value));
      workers[pos(key)]->pushRequest(r);
      // No response
      break;
    }
    case TxRequest_Request::GETRANGE: {
      cout << "GETRANGE" << endl;
      int32 start = it->key1();
      int32 end = it->key2();
      int startPos = pos(start);
      int endPos = pos(end);

      if (startPos == endPos) {
        RequestPtr r = RequestPtr(new Request(tx));
        r -> pushOp(Request::createGetRangeOp(start, end));
        workers[startPos]->pushRequest(r);
      } else {
        RequestPtr r = RequestPtr(new Request(tx));
        r -> pushOp(Request::createGetRangeOp(start, (startPos + 1) * size + down - 1));
        workers[startPos]->pushRequest(r);

        for (int i = startPos + 1; i < endPos; ++i) {
          RequestPtr r = RequestPtr(new Request(tx));
          r -> pushOp(Request::createGetRangeOp(down + i * size, down + (i + 1) * size - 1));
          workers[i]->pushRequest(r);
        }

        RequestPtr r2 = RequestPtr(new Request(tx));
        r2 -> pushOp(Request::createGetRangeOp(down + endPos * size, end));
        workers[endPos]->pushRequest(r2);
      }
      break;
    }
    }
  }
  tx->wait();
  std::map<int, std::string> m = tx -> getResults();
  std::map<int, std::string>::iterator it2 = m.begin();
  TxResponse_Map * ret = response->add_retvalue();
  for (; it2 != m.end(); ++it) {
    ret->set_key(it2->first);
    ret->set_value(it2->second);
  }

  done->Run();
}
