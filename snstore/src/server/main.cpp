#include <coordinator.h>
#include <iostream>
#include <map>

typedef boost::shared_ptr<Transaction> TransactionPtr;
typedef boost::shared_ptr<Request> RequestPtr;

int main() {
  	Coordinator coor(5,0, 100);//initial for test
	return 0;
}
