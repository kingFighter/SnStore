#ifndef REQUEST_H
#define REQUEST_H

#include "transaction.hpp" 
#include "../RCF/operation.hpp"
#include <vector> 
#include <string>
#include <queue>
#include <boost/shared_ptr.hpp>


class Request {
public:
	typedef boost::shared_ptr<Transaction> TransactionPtr;
	Request(TransactionPtr t);
	static Operation createGetOp(int key);
	static Operation createPutOp(int key, int value);
	static Operation createGetRangeOp(int begin, int end);

	void pushOp(const Operation& o);
	void addResult(int key, const int value);
	void done();
	const Operation popOp();
	bool empty();
private:
	TransactionPtr tx;
	std::queue<Operation> operations;
};

#endif
