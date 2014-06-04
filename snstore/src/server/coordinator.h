#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <RCFProto.hpp>
#include "../RCFProto/snstore.pb.h"
#include <map>
#include <vector>
#include <string>
#include <pthread.h>

using namespace google::protobuf;

class Coordinator : public DbService{
public:
	Coordinator(int worker_num, int max_key);
	~Coordinator();
    void get(RpcController * controller, const GetRequest * request, GetResponse * response, Closure * done );
    void put(RpcController * controller, const PutRequest * request, PutResponse * response, Closure * done );
    void getrange(RpcController * controller, const GRRequest * request, GRResponse * response, Closure * done );

private:
//	thread[] workerThreads;
//
//	int execTx(std::string args, map<int, std::string>& retVal);
//	int local_execTx(std::string args, Transaction r);
};
#endif
