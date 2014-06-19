#include "worker.hpp"
#include "requestQueue.hpp"
#include "transaction.hpp"
#include <iostream>
#include <map>

typedef boost::shared_ptr<Transaction> TransactionPtr;
typedef boost::shared_ptr<Request> RequestPtr;

int main() {
	Worker work(1, 10, RequestQueue::WAIT_FREE);
	TransactionPtr tx = TransactionPtr(new Transaction());
	RequestPtr r = RequestPtr(new Request(tx));
	r -> pushOp(Request::createPutOp(1, "hello"));
	r -> pushOp(Request::createPutOp(2, "world"));
	r -> pushOp(Request::createGetRangeOp(1, 2));
	work.pushRequest(r);
	tx -> wait();
	std::map<int, std::string> m = tx -> getResults();
	std::cout << m[1] << std::endl << m[2] << std::endl;

	TransactionPtr tx2 = TransactionPtr(new Transaction());
	RequestPtr r2 = RequestPtr(new Request(tx2));
	r2 -> pushOp(Request::createPutOp(1, "fuck"));
	r2 -> pushOp(Request::createGetOp(1));
	work.pushRequest(r2);
	tx2 -> wait();
	m = tx2 -> getResults();
	std::cout << m[1] << std::endl;

	return 0;
}
