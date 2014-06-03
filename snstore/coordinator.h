#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <RCFProto.hpp>
#include "snstore.pb.h"
#include <map>
#include <vector>
#include <string>

using namespace google::protobuf;

class Coordinator : public DbService{
public:
    void get(RpcController * controller, const GetRequest * request, GetResponse * response, Closure * done );
    void put(RpcController * controller, const PutRequest * request, PutResponse * response, Closure * done );
    void getrange(RpcController * controller, const GRRequest * request, GRResponse * response, Closure * done );
};
#endif
