#include "snstore.h"

using namespace Kevin;
using namespace google::protobuf;

SnStore::SnStore() {
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
    if (current_request.istx()) {
        cout<<"cannot begin transaction that has started"<<endl;
        return;
    }
    try
    {
        BeginRequest request;
        BeginResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug("strRequest << std::endl");

        // Make a synchronous remote call to server.
        stub.begin(NULL,&request, &response, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);

        current_request.set_txid(response.txid());
		current_request.set_istx(true);
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return ;
    }

}

void SnStore::commit()
{
    if (!current_request.istx())
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
        current_request.clear_txid();
		current_request.clear_istx();
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        current_request.clear_reqs();
        current_request.clear_txid();
		current_request.clear_istx();
        return ;
    }
}

string SnStore::get(int key) {
    if (current_request.istx() && cache.count(key) != 0) {
        return cache[key];
    }
    try
    {
        TxRequest request;
		request.set_istx(false);
        TxRequest_Request * req = request.add_reqs();
        req->set_op(TxRequest_Request::GET);
        req->set_key1(key);

        TxResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);

        // Make a synchronous remote call to server.
        stub.execTx(NULL,&request, &response, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
        RepeatedPtrField<TxResponse_Map> ret = response.retvalue();
        RepeatedPtrField<TxResponse_Map>::iterator it = ret.begin();

        if (current_request.istx()) { //this is a transaction request
            if (it->value() != "")//the value exists
                cache[key] = it->value();
        }
        return it->value();
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return "";
    }
}

void SnStore::put(int key, string value) {
    TxRequest_Request * req = current_request.add_reqs();
    req->set_op(TxRequest_Request::PUT);
    req->set_key1(key);
    req->set_value(value);

    if (current_request.istx()) {//this is a transaction request
        cache[key] = value;
        return;
    }

    try
    {
		current_request.set_istx(false);
        TxResponse rsponse;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(current_request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);

        // Make a synchronous remote call to server.
        stub.execTx(NULL,&current_request, &rsponse, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(rsponse, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
        current_request.clear_reqs();
		current_request.clear_istx();
    }
    catch(const RCF::Exception & e)
    {
        Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        current_request.clear_reqs();
		current_request.clear_istx();
        return ;
    }
}
vector<string> SnStore::getRange(int minkey, int maxkey) {
    vector<string> v;
    if (current_request.istx()) {//this is a transaction request
        for (int i = minkey; i <= maxkey ;i++) {
            if (cache.count(i) != 0) {
                v.push_back(cache[i]);
                if (i == maxkey)
                    return v;
            }
            else {
                v.clear();
                break;
            }
        }
    }

    try
    {
        TxRequest request;
		request.set_istx(false);
        TxRequest_Request * req = request.add_reqs();
        req->set_op(TxRequest_Request::GETRANGE);
        req->set_key1(minkey);
        req->set_key2(maxkey);

        TxResponse rsponse;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);
        // Make a synchronous remote call to server.
        stub.execTx(NULL,&request, &rsponse, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(rsponse, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
        RepeatedPtrField<TxResponse_Map> ret = rsponse.retvalue();
        RepeatedPtrField<TxResponse_Map>::iterator it = ret.begin();
        for(; it != ret.end(); it++) {
            if (current_request.istx()) {//transaction
                if (it->value() != "") {//\ and not ""
                    cache[it->key()] = it->value();
                }
                if (cache.count(it->key()) != 0)
                    v.push_back(cache[it->key()]);
                else
                    v.push_back("");
            }
            else
                v.push_back(it->value());

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
