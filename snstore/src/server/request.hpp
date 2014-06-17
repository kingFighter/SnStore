#ifndef REQUEST_H
#define REQUEST_H
#include "transaction.hpp" 
#include <vector> 
#include <string>

class Request {
enum Type {
	get = 0,
	put,
	getRange
};

struct Operation {
	Type type;
	int key;
	std::string value;
	int max;
	int min;
};

public:
	Request(Transaction& t);
	Request();
	Request(Request& r);
	void addOperation(const Operation& o);
	void putResult(int key, const std::string& value);
private:
	Transaction tx;
	std::vector<Operation> operations;
};

#endif
