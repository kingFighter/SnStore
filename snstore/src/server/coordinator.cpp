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
  processThread = boost::thread(&Coordinator::processResults, this);
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
        string value = it->value();
        ++num;
        string str = "put " + int2string(putkey) + " " + value;
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
        
        for (int i = posMin + 1; i < posMax; ++i) {
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
            ret->set_value(reGet[getkey]);
            break;
          }
        case TxRequest_Request::PUT: {
            break;
          }
        case TxRequest_Request::GETRANGE: {
            int minkey = it->key1();
            int maxkey = it->key2();
            minMaxV.push_back(make_pair(minkey, maxkey));
            vector<string> v = reGetRange[make_pair(minkey, maxkey)];
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

void Coordinator::processResults() {
  while (true) {
    {
      
      boost::mutex::scoped_lock lock(results_mutex);
      // We can improve this later.
      bool empty = true;
      while (empty) {
        for (int i = 0; i < results.size(); ++i) {
          if (!results[i].empty()) {
            empty = false;
            break;
          }
        }
        if (empty) 
          results_con.wait(lock);
      }

      Debug("Server process results.\n");
      for (int i = 0; i < results.size(); ++i) {
        if (!((results[i]).empty())) {
          --num;
        }
        while (!((results[i]).empty())) {
          Debug("Server Processing.\n");
          string str = results[i].front();
          results[i].pop();
          istringstream iss(str);
          vector<string> tokens;
          copy(istream_iterator<string>(iss),
               istream_iterator<string>(),
               back_inserter<vector<string> >(tokens));

          if ("get" == tokens[0]) {
            reGet[string2int(tokens[1])] = tokens[2];
          } else if ("put" == tokens[0]) {
            // nothing to do
          } else if ("getRange" == tokens[0]) {
            vector<string> getRangeValues (tokens.begin() + 3, tokens.end());
            reGetRange[make_pair(string2int(tokens[1]), string2int(tokens[2]))] = getRangeValues;
          }
        } // end while results[i].empty()
      }   // end for results.size()
      
      for (int i = 0; i < minMaxV.size(); ++i) {
        pair<int, int> mmv = minMaxV[i];
        // Across worker
        if (0 == reGetRange.count(mmv)) {
          int minKey = mmv.first;
          int maxKey = mmv.second;
          
          int posMin = getPos(minKey);
          int posMax = getPos(maxKey);
          num += posMax - posMin + 1;
          
          pair<int, int> p = make_pair(minKey, (posMin * size - 1));
          vector<string> getRangeValues(reGetRange[p].begin(), reGetRange[p].end());
          for (int i = posMin + 1; i < posMax; ++i) {
            p = make_pair((i - 1) * size, i * size - 1);
            getRangeValues.insert(getRangeValues.end(), reGetRange[p].begin(), reGetRange[p].end());
          }
          p = make_pair((posMax - 1) * size, maxKey);
          getRangeValues.insert(getRangeValues.end(), reGetRange[p].begin(), reGetRange[p].end());
          reGetRange[mmv] = getRangeValues;
        } // end reGetRange.count(mmv)
      }
      request_con.notify_all();
      Debug("Server process done\n");
    }
  }

}
