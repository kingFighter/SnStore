#include "snstore.h"

using namespace Kevin;
using namespace google::protobuf;

void onCommit(RCF::RcfProtoController *pController, std::pair<TxRequest *, TxResponse *> args) {
    if (pController->Failed()) {
        std::cout << "RPC error: " << pController->ErrorText() << std::endl;
    }
    else {
        TxRequest * request = args.first;
        TxResponse * response = args.second;
        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(*response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);

        RepeatedPtrField<TxResponse_Map> rst = response->retvalue();
        RepeatedPtrField<TxResponse_Map>::iterator it = rst.begin();
        for(; it != rst.end(); it++)
          Debug(it->key()<<","<<it->value()<<std::endl);
    }
}

void onGet(RCF::RcfProtoController *pController, std::pair<GetRequest *, GetResponse *> args) {
    if (pController->Failed()) {
        std::cout << "RPC error: " << pController->ErrorText() << std::endl;
    }
    else {
        GetRequest * request = args.first;
        GetResponse * response = args.second;
        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(*response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
    }
}

void onPut(RCF::RcfProtoController *pController, std::pair<PutRequest *, PutResponse *> args) {
    if (pController->Failed()) {
        std::cout << "RPC error: " << pController->ErrorText() << std::endl;
    }
    else {
        PutRequest * request = args.first;
        PutResponse * response = args.second;
        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(*response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
    }
}

void onGetRange(RCF::RcfProtoController *pController, std::pair<GRRequest *, GRResponse *> args) {
    vector<string> v;
    if (pController->Failed()) {
        std::cout << "RPC error: " << pController->ErrorText() << std::endl;
    }
    else {
        GRRequest * request = args.first;
        GRResponse * response = args.second;
        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(*response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
        RepeatedPtrField<string> ret = response->value();
        RepeatedPtrField<string>::iterator it = ret.begin();
        for(; it != ret.end(); it++) {
            v.push_back(*it);
        }
    }
}

SnStore::SnStore() {
    istx = false;
    try
    {
        RCF::init();
    }
    catch(const RCF::Exception & e)
    {
      Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return ;
    }
}

void SnStore::beginTx()
{
    if (istx) {
        cout<<"cannot begin transaction that has started"<<endl;
        return;
    }
    istx = true;
}

void SnStore::commit()
{
    if (!istx)
        return;

    //call rpc to execute transaction
    try
    {
        TxResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(current_request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug("strRequest << std::endl");

        // Make a synchronous remote call to server.
        stub.execTx(NULL,&current_request, &response, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);

        RepeatedPtrField<TxResponse_Map> rst = response.retvalue();
        RepeatedPtrField<TxResponse_Map>::iterator it = rst.begin();
        for(; it != rst.end(); it++)
          Debug(it->key()<<","<<it->value()<<std::endl);
        current_request.clear_reqs();
        istx = false;
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        current_request.clear_reqs();
        return ;
    }
}

void SnStore::commit_async() {
    if (!istx)
        return;

    //call rpc to execute transaction
    try
    {
        TxResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(current_request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug("strRequest << std::endl");

        // Make a asynchronous remote call to server.
        RCF::RcfProtoController controller;
        Closure * pClosure = NewCallback(&onCommit, &controller, std::make_pair(&current_request, &response));
        stub.execTx(&controller,&current_request, &response, pClosure);

        current_request.clear_reqs();
        istx = false;
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        current_request.clear_reqs();
        return ;
    }
}

string SnStore::get(int key) {
    if (istx) {
        TxRequest_Request * req = current_request.add_reqs();
        req->set_op(TxRequest_Request::GET);
        req->set_key1(key);
        return "";
    }
    try
    {
        GetRequest request;
        request.set_key(key);

        GetResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);

        // Make a synchronous remote call to server.
        stub.get(NULL,&request, &response, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);

        return response.value();
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return "";
    }
}

void SnStore::get_async(int key) {
    if (istx) {
        TxRequest_Request * req = current_request.add_reqs();
        req->set_op(TxRequest_Request::GET);
        req->set_key1(key);
        return;
    }
    try
    {
        GetRequest request;
        request.set_key(key);

        GetResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);

        // Make a asynchronous remote call to server.
        RCF::RcfProtoController controller;
        Closure * pClosure = NewCallback(&onGet, &controller, std::make_pair(&request, &response));
        stub.get(&controller,&request, &response, pClosure);
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return ;
    }
}

void SnStore::put(int key, string value) {
    if (istx) {//this is a transaction request
        TxRequest_Request * req = current_request.add_reqs();
        req->set_op(TxRequest_Request::PUT);
        req->set_key1(key);
        req->set_value(value);
        return;
    }

    try
    {
        PutRequest request;
        request.set_key(key);
        request.set_value(value);
        PutResponse rsponse;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);

        // Make a synchronous remote call to server.
        stub.put(NULL,&request, &rsponse, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(rsponse, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return ;
    }
}

void SnStore::put_async(int key, string value) {
    if (istx) {//this is a transaction request
        TxRequest_Request * req = current_request.add_reqs();
        req->set_op(TxRequest_Request::PUT);
        req->set_key1(key);
        req->set_value(value);
        return;
    }

    try
    {
        PutRequest request;
        request.set_key(key);
        request.set_value(value);
        PutResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);

        // Make a asynchronous remote call to server.
        RCF::RcfProtoController controller;
        Closure * pClosure = NewCallback(&onPut, &controller, std::make_pair(&request, &response));
        stub.put(&controller,&request, &response, pClosure);
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return ;
    }
}

vector<string> SnStore::getRange(int minkey, int maxkey) {
    vector<string> v;
    if (istx) {//this is a transaction request
        TxRequest_Request * req = current_request.add_reqs();
        req->set_op(TxRequest_Request::GETRANGE);
        req->set_key1(minkey);
        req->set_key2(maxkey);
        return v;
    }

    try
    {
        GRRequest request;
        request.set_start(minkey);
        request.set_end(maxkey);

        GRResponse rsponse;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);
        // Make a synchronous remote call to server.
        stub.getrange(NULL,&request, &rsponse, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(rsponse, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
        RepeatedPtrField<string> ret = rsponse.value();
        RepeatedPtrField<string>::iterator it = ret.begin();
        for(; it != ret.end(); it++) {
            v.push_back(*it);
        }
        return v;
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return v;
    }
    return v;
}

void SnStore::getRange_async(int minkey, int maxkey) {
    if (istx) {//this is a transaction request
        TxRequest_Request * req = current_request.add_reqs();
        req->set_op(TxRequest_Request::GETRANGE);
        req->set_key1(minkey);
        req->set_key2(maxkey);
        return;
    }

    try
    {
        GRRequest request;
        request.set_start(minkey);
        request.set_end(maxkey);

        GRResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);
        // Make a asynchronous remote call to server.
        RCF::RcfProtoController controller;
        Closure * pClosure = NewCallback(&onGetRange, &controller, std::make_pair(&request, &response));
        stub.getrange(&controller,&request, &response, pClosure);

        return;
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return;
    }
    return;
}
