#ifndef DEBUG_H
#define DEBUG_H

#define DEFAULT_TX_ID 0

#ifdef USEDEBUG
#include <iostream>
#define Debug(x) std::cerr << x
#define Warn(x)  std::cout << x
#else
#define Debug(x) 
#endif 

#endif
