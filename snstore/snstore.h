#ifndef SNSTORE_H
#define SNSTORE_H
#include <vector>
#include <string>
using namespace std;
namespace Kevin {
  class SnStore {
  public:
string get(int key){return "";}
    void put(int key, string value){}
    vector<string> getRange(int minKey, int maxKey){vector<string> v;return v;}
  };
}
#endif
