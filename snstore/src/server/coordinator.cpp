#include "coordinator.h"

std::map<int,std::string> db;

Coordinator::Coordinator(int worker_num, int max_key)
{

}

Coordinator::~Coordinator()
{
}

void
Coordinator::get(RpcController * controller, const GetRequest * request, GetResponse * response, Closure * done)
{
    int key = request->key();
    if(db.count(key) == 0)
        response->set_value(NULL);
    else
        response->set_value(db[key]);
    done->Run();
}
void
Coordinator::put(RpcController * controller, const PutRequest * request, PutResponse * response, Closure * done )
{
    int key = request->key();
    string value = request->value();
    db[key] = value;
    //std::cout<<"try to put " <<value<<std::endl;
    response->set_result(true);
    done->Run();
}
void
Coordinator::getrange(RpcController * controller, const GRRequest * request, GRResponse * response, Closure * done )
{
    int start = request->start();
    int end_ =  request ->end();
    response->set_result(true);
    for(int i = start; i <= end_; i++) {
        if(db.count(i) != 0)
            response->add_value(db[i]);
    }
    done->Run();
}
void
Coordinator::execTx(RpcController* controller, const TxRequest* request, TxResponse* response, Closure* done)
{
    string args = request->args();
    //parse the args into several req
    char* req;
    char* tx_args_c = new char[args.length() + 1];
    strcpy(tx_args_c, args.c_str());
    req = strtok(tx_args_c, ";");
    while(req != NULL)//handle every request
    {
      Debug(req<<std::endl);
        req = strtok(NULL, ";");
        TxResponse_map* map = response->add_retvalue();
        map->set_key(1);
        map->set_value("2");
    }
    done->Run();
}
