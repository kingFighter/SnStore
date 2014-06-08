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

using namespace google::protobuf;

class Coordinator : public DbService{
public:
  Coordinator(int worker_num, int down, int up);
	~Coordinator();
	void begin(RpcController* controller, const BeginRequest* request, BeginResponse* response, Closure* done);
    void execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done);

private:
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
    vector<queue<string> > operations;
    vector<queue<string> > results;
    vector<boost::condition_variable> conditions;
    /* global_mutex used as global lock */
    boost::mutex global_mutex;
    boost::mutex results_mutex;
    boost::mutex request_mutex;
    boost::condition_variable request_con;
    boost::condition_variable results_con;
    boost::condition_variable global_con;
    int getPos(int key) {
      if (key < down) {
        return 0;
      } else if (key >= up) {
        return workers.size() - 1;
      } else {
        return (key / size + 1);
      }
      
    }
};
#endif








