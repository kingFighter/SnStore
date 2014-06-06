#ifndef DEBUG_H
#define DEBUG_H

#ifdef USEDEBUG
#include <iostream>
#define Debug(x) std::cerr << x
#define Warn(x)  std::cout << x
#else
#define Debug(x) 
#endif 

#endif
