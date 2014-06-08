#include "coordinator.h"

std::map<int,std::string> db;

Coordinator::Coordinator(int worker_num, int down_, int up_) : down(down_), up(up_)
{
  txid = DEFAULT_TX_ID;
  holder = DEFAULT_TX_ID;
  size = (up - down) / (worker_num - 2);
  // worker[0]: (~,down)
  // worker[1]: [down, down + size)
  // worker[2]: [down + size, down + 2 * size)
  // worker[worker_num - 2]: [up,~)
  // conditions.resize(worker_num);
  for (int i = 0; i < worker_num; ++i) {
    workers.push_back(Worker());
    operations.push_back(queue<string>());
    results.push_back(queue<string>());
  }
}

Coordinator::~Coordinator()
{
}

void
Coordinator::begin(RpcController* controller, const BeginRequest* request, BeginResponse* response, Closure* done)
{
  if (holder == DEFAULT_TX_ID)
    ++holder;
    response->set_txid(++txid);
    done->Run();
}

void
Coordinator::execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done)
{
  int32 txidReqs = request->txid();
  // get transaction lock
  {
    boost::mutex::scoped_lock lock(global_mutex);
    while (txidReqs != holder)
      global_con.wait(lock);
  }
  {
    boost::mutex::scoped_lock lock(request_mutex);
    RepeatedPtrField<TxRequest_Request> reqs = request->reqs();
    RepeatedPtrField<TxRequest_Request>::iterator it = reqs.begin();
    
    // split request
    // For now the operationsWakeup is useless.
    set<int> operationsWakeup;
    for(;it != reqs.end(); it++) {
      switch (it->op()) {
      case TxRequest_Request::GET: {
        int getkey = it->key1();
        ++num;
        string str = "get " + int2string(getkey);
        int pos = getPos(getkey);
        operationsWakeup.insert(pos);
        (operations[pos]).push(str);
        break;
      }
      case TxRequest_Request::PUT: {
        int putkey = it->key1();
        ++num;
        string str = "put " + int2string(putkey);
        int pos = getPos(putkey);
        operationsWakeup.insert(pos);
        (operations[pos]).push(str);
        break;
      }
      case TxRequest_Request::GETRANGE: {
        int minkey = it->key1();
        int maxkey = it->key2();
        // number and split
        int posMin = getPos(minkey);
        int posMax = getPos(maxkey);
        num += posMax - posMin + 1;
        // Split getRange
        string str = "getRange ";
        if (posMin != posMax) {
          (operations[posMin]).push(str + int2string(minkey) + " " + int2string(posMin * size - 1));
          operationsWakeup.insert(posMin);
          (operations[posMax]).push(str + int2string((posMax - 1) * size) + " " + int2string(maxkey));
          operationsWakeup.insert(posMax);
        } else {
          operations[posMin].push(str + int2string(minkey) + " " + int2string(maxkey));
          operationsWakeup.insert(posMin);
        }
        
        for (int i = posMin + 1; i <= posMax; ++i) {
          operationsWakeup.insert(i);
          (operations[i]).push(str + int2string((i - 1) * size ) + " " + int2string(i * size - 1));
        }
        break;
      }
      }
    }
    operations_con.notify_all();
    {
      boost::mutex::scoped_lock lock(request_mutex);
      while (num > 0)
        request_con.wait(lock);

      // Deal with results
      for(;it != reqs.end(); it++) {
        switch (it->op()) {
        case TxRequest_Request::GET: {
            int getkey = it->key1();
            TxResponse_Map* ret = response->add_retvalue();
            ret->set_key(getkey);
            ret->set_value(reGet.front());
            reGet.pop();
            break;
          }
        case TxRequest_Request::PUT: {
            break;
          }
        case TxRequest_Request::GETRANGE: {
            int minkey = it->key1();
            int maxkey = it->key2();
            vector<string> v = reGetRange.front();
            reGetRange.pop();
            for (int i = 0; i < v.size(); ++i) {
              TxResponse_Map * ret = response->add_retvalue();
              ret->set_key(i + minkey);
              ret->set_value(v[i]);
            }
            break;
          }
        } // end switch
      } // end for
    } // end scope
    done->Run();
  }
}
