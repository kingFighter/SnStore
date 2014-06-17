#ifndef REQUEST_H
#define REQUEST_H
#include "transaction.hpp" 
#include <vector> 
#include <string>
#include <queue>
#include <boost/shared_ptr.hpp>

class Request {
public:
	typedef boost::shared_ptr<Transaction> TransactionPtr;
enum Type {
	GET = 0,
	PUT,
	GETRANGE
};

struct Operation {
	Type type;
	int key;
	std::string value;
	int max;
	int min;
};

	Request(TransactionPtr t);

	void pushOperation(const Operation& o);
	void addResult(int key, const std::string& value);
	const Operation popOperation();
private:
	TransactionPtr tx;
	std::queue<Operation> operations;
};

#endif
