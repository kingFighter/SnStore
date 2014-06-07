#include "coordinator.h"

std::map<int,std::string> db;

Coordinator::Coordinator(int worker_num, int max_key)
{

}

Coordinator::~Coordinator()
{
}

void
Coordinator::begin(RpcController* controller, const BeginRequest* request, BeginResponse* response, Closure* done)
{
    int id = request->txid();
    done->Run();
}

void
Coordinator::execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done)
{
    RepeatedPtrField<TxRequest_Request> reqs = request->reqs();
    RepeatedPtrField<TxRequest_Request>::iterator it = reqs.begin();
    for(;it != reqs.end(); it++) {
        switch (it->op()) {
            case TxRequest_Request::GET:
            {
                int getkey = it->key1();
                break;
            }
            case TxRequest_Request::PUT:
            {
                int putkey = it->key1();
                string value = it->value();
                break;
            }
            case TxRequest_Request::GETRANGE:
            {
                int minkey = it->key1();
                int maxkey = it->key2();
                break;
            }
        }
    }
    done->Run();
}
