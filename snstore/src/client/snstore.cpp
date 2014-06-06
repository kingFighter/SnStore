#include "snstore.h"

using namespace Kevin;
using namespace google::protobuf;

SnStore::SnStore() {
    tx = false;
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
    tx = true;
}

void SnStore::commit()
{
    if (!tx)
        return;
    tx = false;
    Debug(tx_args<<std::endl);

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
    }
    catch(const RCF::Exception & e)
    {
      Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return ;
    }

}

string SnStore::get(int key) {
    TxRequest_Request * req = current_request.add_reqs();
    req->set_op(TxRequest_Request::GET);
    req->set_key1(key);
    if (tx)
    {
        return "";
    }
    try
    {
        TxResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(current_request, &strRequest);
        Debug("Sending request:" << std::endl);
        Debug(strRequest << std::endl);

        // Make a synchronous remote call to server.
        stub.execTx(NULL,&current_request, &response, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(response, &strResponse);
        Debug("Received response:" << std::endl);
        Debug(strResponse << std::endl);
        RepeatedPtrField<TxResponse_Map> ret = response.retvalue();
        RepeatedPtrField<TxResponse_Map>::iterator it = ret.begin();
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
    if (tx)
    {
        return;
    }
    try
    {
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
    }
    catch(const RCF::Exception & e)
    {
      Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return ;
    }
}
vector<string> SnStore::getRange(int minkey, int maxkey) {
    vector<string> v;
    TxRequest_Request * req = current_request.add_reqs();
    req->set_op(TxRequest_Request::GETRANGE);
    req->set_key1(minkey);
    req->set_key2(maxkey);
    if (tx)
    {
        return v;
    }
    try
    {
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
        RepeatedPtrField<TxResponse_Map> ret = rsponse.retvalue();
        RepeatedPtrField<TxResponse_Map>::iterator it = ret.begin();
        for(; it != ret.end(); it++)
            v.push_back(it->value());
        return v;
    }
    catch(const RCF::Exception & e)
    {
      Debug("RCF::Exception: " << e.getErrorString() << std::endl);
        return v;
    }
    return v;
}
