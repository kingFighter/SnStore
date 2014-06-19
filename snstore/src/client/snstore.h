#ifndef SNSTORE_H
#define SNSTORE_H
#include <vector>
#include <string>
#include <map>
#include <RCFProto.hpp>

#include "../RCFProto/snstore.pb.h"
#include <google/protobuf/text_format.h>
#include "../utility/utility.h"

using namespace std;
namespace Kevin {
  class SnStore {
  public:
    SnStore();
    string get(int key);
    void put(int key, string value);
    vector<string> getRange(int minkey, int maxkey);
    void beginTx();
    void commit();
    //Asynchronous remote call.
    void get_async(int key);
    void put_async(int key, string value);
    void getRange_async(int minkey, int maxkey);
    void commit_async();
  private:
    TxRequest current_request;
    bool istx;
  };
}
#endif
