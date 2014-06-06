#ifndef SNSTORE_H
#define SNSTORE_H
#include <vector>
#include <string>
#include <RCFProto.hpp>
#include "../RCFProto/snstore.pb.h"
#include <google/protobuf/text_format.h>
#include "../utility/debug.h"

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
  private:
    bool tx;
    TxRequest current_request;
  };
}
#endif
