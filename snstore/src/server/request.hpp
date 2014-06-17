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
	int begin;
	int end;
};

	Request(TransactionPtr t);
	static Operation createGetOp(int key);
	static Operation createPutOp(int key, const std::string& value);
	static Operation createGetRangeOp(int begin, int end);

	void pushOp(const Operation& o);
	void addResult(int key, const std::string value);
	void done();
	const Operation popOp();
	bool empty();
private:
	TransactionPtr tx;
	std::queue<Operation> operations;
};

#endif
