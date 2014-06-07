#include "coordinator.h"

std::map<int,std::string> db;

Coordinator::Coordinator(int worker_num, int max_key)
{
    txid = 1;
}

Coordinator::~Coordinator()
{
}

void
Coordinator::begin(RpcController* controller, const BeginRequest* request, BeginResponse* response, Closure* done)
{
    response->set_txid(txid++);
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
                TxResponse_Map* ret = response->add_retvalue();
                ret->set_key(getkey);
                if (db.count(getkey) != 0)
                    ret->set_value(db[getkey]);
                else
                    ret->set_value("");
                break;
            }
            case TxRequest_Request::PUT:
            {
                int putkey = it->key1();
                string value = it->value();
                db[putkey] = value;
                break;
            }
            case TxRequest_Request::GETRANGE:
            {
                int minkey = it->key1();
                int maxkey = it->key2();
                for (int i = minkey; i <= maxkey; i++) {
                    TxResponse_Map * ret = response->add_retvalue();
                    ret->set_key(i);
                    if (db.count(i) != 0) {
                        ret->set_value(db[i]);
                    }
                    else
                        ret->set_value("");
                }
                break;
            }
        }
    }
    done->Run();
}
