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

using namespace google::protobuf;

class Coordinator : public DbService{
public:
  Coordinator(int worker_num, int down, int up);
	~Coordinator();
	void begin(RpcController* controller, const BeginRequest* request, BeginResponse* response, Closure* done);
    void execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done);

private:
    int txid;
    int down;
    int up;
    vector<Worker> workers;
//
//	int local_execTx(std::string args, Transaction r);
};
#endif






