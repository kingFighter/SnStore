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
    int get(int key);
    void put(int key, int value);
    vector<int> getRange(int minkey, int maxkey);
    void beginTx();
    map<int,int> commit();
    //Asynchronous remote call.
    void get_async(int key);
    void put_async(int key, int value);
    void getRange_async(int minkey, int maxkey);
    void commit_async();
  private:
    TxRequest current_request;
    bool istx;
  };
}
#endif
