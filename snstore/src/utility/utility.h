#ifndef DEBUG_H
#define DEBUG_H
#include <iostream>
#include <sstream>
#include <string>
using std::string;

#define DEFAULT_TX_ID 0

#ifdef USEDEBUG
#define Debug(x) std::cerr << x
#define Warn(x)  std::cout << x
#else
#define Debug(x) 
#endif 

inline int string2int(const string &str) {
  std::istringstream istr(str);
  int num;
  istr >> num;
  return num;
}

inline string int2string(int num) {
  std::ostringstream ostr;
  ostr << num;
  string str;
  str = ostr.str();
  return str;
}
#endif
