#include "snstore.h"
#include <RCFProto.hpp>
#include "../RCFProto/snstore.pb.h"
#include <google/protobuf/text_format.h>

using namespace Kevin;
using namespace google::protobuf;

SnStore::SnStore() {
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

string SnStore::get(int key) {
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
