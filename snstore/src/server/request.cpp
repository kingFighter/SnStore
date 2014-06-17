#include "request.hpp"

Request::Request(boost::shared_ptr<Transaction> t) : tx(t){
}

void Request::addResult(int key, const std::string& value) {
	tx -> addResult(key, value);
}

void Request::pushOperation(const Operation& o) {
	operations.push(o);
}

const Request::Operation Request::popOperation() {
	Operation pop = operations.front();
	operations.pop();
	return pop;
}

