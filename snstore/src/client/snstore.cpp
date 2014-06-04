#include "snstore.h"

using namespace Kevin;
using namespace google::protobuf;

SnStore::SnStore() {
    tx = false;
    tx_args = "";
    try
    {
        RCF::init();
    }
    catch(const RCF::Exception & e)
    {
        std::cout << "RCF::Exception: " << e.getErrorString() << std::endl;
        return ;
    }
}

void SnStore::beginTx()
{
    tx = true;
    tx_args = "";
}

void SnStore::commit()
{
    tx = false;
    std::cout<<tx_args<<std::endl;

    //call rpc to execute transaction
    try
    {
        TxRequest request;
        request.set_args(tx_args);

        TxResponse response;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        std::cout << "Sending request:" << std::endl;
        std::cout << strRequest << std::endl;

        // Make a synchronous remote call to server.
        stub.execTx(NULL,&request, &response, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(response, &strResponse);
        std::cout << "Received response:" << std::endl;
        std::cout << strResponse << std::endl;

        RepeatedPtrField<TxResponse_map> rst = response.retvalue();
        RepeatedPtrField<TxResponse_map>::iterator it = rst.begin();
        for(; it != rst.end(); it++)
            std::cout<<it->key()<<","<<it->value()<<std::endl;
    }
    catch(const RCF::Exception & e)
    {
        std::cout << "RCF::Exception: " << e.getErrorString() << std::endl;
        return ;
    }


    tx_args = "";
}

string SnStore::get(int key) {
    if (tx)
    {
        std::ostringstream oss;
        oss<<key;
        tx_args += "get " + oss.str() + ";";
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
        std::cout << "Sending request:" << std::endl;
        std::cout << strRequest << std::endl;

        // Make a synchronous remote call to server.
        stub.get(NULL,&request, &response, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(response, &strResponse);
        std::cout << "Received response:" << std::endl;
        std::cout << strResponse << std::endl;
        return response.value();
    }
    catch(const RCF::Exception & e)
    {
        std::cout << "RCF::Exception: " << e.getErrorString() << std::endl;
        return "";
    }
}

void SnStore::put(int key, string value) {
    if (tx)
    {
        std::ostringstream oss;
        oss<<key;
        tx_args += "put " + oss.str() + " ";
        oss.clear();
        oss<<value;
        tx_args += oss.str()+ ";";
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
        std::cout << "Sending request:" << std::endl;
        std::cout << strRequest << std::endl;

        // Make a synchronous remote call to server.
        stub.put(NULL,&request, &rsponse, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(rsponse, &strResponse);
        std::cout << "Received response:" << std::endl;
        std::cout << strResponse << std::endl;
    }
    catch(const RCF::Exception & e)
    {
        std::cout << "RCF::Exception: " << e.getErrorString() << std::endl;
        return ;
    }
}
vector<string> SnStore::getRange(int minKey, int maxKey) {
    vector<string> v;
    if (tx)
    {
        std::ostringstream oss;
        oss<<minKey;
        tx_args += "getrange " + oss.str();
        oss.clear();
        oss<<maxKey;
        tx_args += " "+ oss.str()+ ";";
        return v;
    }
    try
    {
        GRRequest request;
        request.set_start(minKey);
        request.set_end(maxKey);

        GRResponse rsponse;
        RCF::RcfProtoChannel channel( RCF::TcpEndpoint("127.0.0.1", 50001) );
        DbService::Stub stub(&channel);

        std::string strRequest;
        TextFormat::PrintToString(request, &strRequest);
        std::cout << "Sending request:" << std::endl;
        std::cout << strRequest << std::endl;

        // Make a synchronous remote call to server.
        stub.getrange(NULL,&request, &rsponse, NULL);

        // Print out response.
        std::string strResponse;
        TextFormat::PrintToString(rsponse, &strResponse);
        std::cout << "Received response:" << std::endl;
        std::cout << strResponse << std::endl;
        RepeatedPtrField< ::std::string> rst = rsponse.value();
        RepeatedPtrField< ::std::string>::iterator it = rst.begin();
        for(; it != rst.end(); it++)
            v.push_back(*it);
        return v;
    }
    catch(const RCF::Exception & e)
    {
        std::cout << "RCF::Exception: " << e.getErrorString() << std::endl;
        return v;
    }
    return v;
}
