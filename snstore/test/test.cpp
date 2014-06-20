#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <map>

#include "snstore.h"
using namespace Kevin;
int main() {
  SnStore db;
	db.put(1, "hello");
	std::cout << db.get(1);
	return 0;
}
