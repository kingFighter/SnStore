#include "coordinator.h"
#include "../RCF/operation.hpp"

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

std::string
Coordinator::get(int key)
{
  TransactionPtr tx = TransactionPtr(new Transaction());
  RequestPtr r = RequestPtr(new Request(tx));
  r -> pushOp(createGetOp(key));
  workers[pos(key)]->pushRequest(r);
  tx->wait();
  std::map<int, std::string> m = tx -> getResults();
  return m[key];
}

int
Coordinator::put(int key, std::string value)
{
  //	std::cout << "Put Request!\nKey:" << key << "\nValue:" << value << "\n";
  TransactionPtr tx = TransactionPtr(new Transaction());
  RequestPtr r = RequestPtr(new Request(tx));
  r -> pushOp(createPutOp(key, value));
  workers[pos(key)]->pushRequest(r);
  tx->wait();
  std::map<int, std::string> m = tx -> getResults();
	return 0;
}

bool
Coordinator::getRange(int start, int end, std::map<int, std::string>& result)
{
	
  int startPos = pos(start);
  int endPos = pos(end);

  TransactionPtr tx = TransactionPtr(new Transaction());

  if (startPos == endPos) {
    RequestPtr r = RequestPtr(new Request(tx));
    r -> pushOp(createGetRangeOp(start, end));
    workers[startPos]->pushRequest(r);
  } else {
    boost::mutex::scoped_lock lock(global_mutex);
    RequestPtr r = RequestPtr(new Request(tx));
    r -> pushOp(createGetRangeOp(start, (startPos + 1) * size + down - 1));
    workers[startPos]->pushRequest(r);

    for (int i = startPos + 1; i < endPos; ++i) {
      RequestPtr r = RequestPtr(new Request(tx));
      r -> pushOp(createGetRangeOp(down + i * size, down + (i + 1) * size - 1));
      workers[i]->pushRequest(r);
    }

    RequestPtr r2 = RequestPtr(new Request(tx));
    r2 -> pushOp(createGetRangeOp(down + endPos * size, end));
    workers[endPos]->pushRequest(r2);
  }

  tx->wait();
  result = tx -> getResults();
	return true;
}


bool
Coordinator::execTx(std::vector<Operation> ops, std::map<int, std::string>& result)
{
  boost::mutex::scoped_lock lock(global_mutex);
  TransactionPtr tx = TransactionPtr(new Transaction());
	std::vector<Operation>::iterator it = ops.begin();
  for(;it != ops.end(); it++) {
    switch (it->type){
    case Operation::GET : {
      Debug("GET" << endl);
      int key = it->key;
      RequestPtr r = RequestPtr(new Request(tx));
      r -> pushOp(createGetOp(key));
      workers[pos(key)]->pushRequest(r);
      break;
    }
    case Operation::PUT: {
      Debug("PUT" << endl);
      int key = it->key;
      string value = it->value;
      RequestPtr r = RequestPtr(new Request(tx));
      r -> pushOp(createPutOp(key, value));
      workers[pos(key)]->pushRequest(r);
      // No response
      break;
    }
    case Operation::GETRANGE: {
      Debug("GETRANGE" << endl);
      int start = it->begin;
      int end = it->end;
      int startPos = pos(start);
      int endPos = pos(end);

      if (startPos == endPos) {
        RequestPtr r = RequestPtr(new Request(tx));
        r -> pushOp(createGetRangeOp(start, end));
        workers[startPos]->pushRequest(r);
      } else {
        RequestPtr r = RequestPtr(new Request(tx));
        r -> pushOp(createGetRangeOp(start, (startPos + 1) * size + down - 1));
        workers[startPos]->pushRequest(r);

        for (int i = startPos + 1; i < endPos; ++i) {
          RequestPtr r = RequestPtr(new Request(tx));
          r -> pushOp(createGetRangeOp(down + i * size, down + (i + 1) * size - 1));
          workers[i]->pushRequest(r);
        }

        RequestPtr r2 = RequestPtr(new Request(tx));
        r2 -> pushOp(createGetRangeOp(down + endPos * size, end));
        workers[endPos]->pushRequest(r2);
      }
      break;
    }
    }
  }
  tx->wait();
  result = tx -> getResults();
	return true;
}
