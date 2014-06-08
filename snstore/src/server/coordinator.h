#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <RCFProto.hpp>
#include "../RCFProto/snstore.pb.h"
#include <map>
#include <vector>
#include <string>
#include <pthread.h>
#include "../utility/utility.h"
#include "worker.h"
#include <boost/thread.hpp>
#include <queue>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>


using namespace google::protobuf;

/* For now the value of db cannot be empty string */
class Coordinator : public DbService{
public:
  Coordinator(int worker_num, int down, int up);
  void start();
	~Coordinator();
	void begin(RpcController* controller, const BeginRequest* request, BeginResponse* response, Closure* done);
    void execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done);

private:
    /* Server only start once */
    int startNum;
    /* give each client a transactgion id*/
    int txid;
    /* holder has the global lock */
    int holder;
    int down;
    int up;
    /* size = (up - down) / (worker_num - 2) */
    int size; 
    /* the request number */
    int num;
    vector<Worker> workers;
    /* each worker has a operations queue, a result queue and an */
    /* operation queue condition  */
    /* operations queue format: */
    /*   get key */
    /*   put key value */
    /*   getRange minKey maxKey */
    vector<queue<string> > operations;
    /* results: receive and combine the results */
    /* results queue format: */
    /*   get key value */
    /*   put false/true */
    /*   getRange minKey maxKey value1 value2 ..  */
    vector<queue<string> > results;
    
    /* get values and out of order */
    map<int, string> reGet;
    /* getRange values  and out of  order */
    map<pair<int, int>, vector<string> > reGetRange;
    /* put values is not needed now */
    /* minMaxV help to getRange */
    vector<pair<int, int> > minMaxV;

    /* vector<boost::condition_variable> conditions; */
    /* We may improvement the performance by conditions */
    boost::condition_variable operations_con;
    /* global_mutex used as global lock */
    boost::mutex global_mutex;
    boost::mutex results_mutex;
    boost::mutex request_mutex;
    boost::condition_variable request_con;
    boost::condition_variable results_con;
    boost::condition_variable global_con;

    boost::thread processThread;

    int getPos(int key) {
      if (key < down) {
        return 0;
      } else if (key >= up) {
        return workers.size() - 1;
      } else {
        return (key / size + 1);
      }
    }
    void initialize() {
      num = 0;
      for (int i = 0; i < operations.size(); ++i) {
        while (!operations[i].empty()) {
          operations[i].pop();
        }
      }

      for (int i = 0; i < results.size(); ++i) {
        while (!results[i].empty()) {
          results[i].pop();
        }
      }
      reGet.clear();
      reGetRange.clear();
      minMaxV.clear();
    }

    void processResults();
};
#endif
