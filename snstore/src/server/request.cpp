#include "request.hpp"
#include <iostream>

Request::Request(boost::shared_ptr<Transaction> t) : tx(t){
}

void Request::addResult(int key, const std::string value) {
	tx -> addResult(key, value);
}

void Request::pushOp(const Operation& o) {
	operations.push(o);
	tx -> pushOp();
}

const Operation Request::popOp() {
	Operation pop = operations.front();
	operations.pop();
	return pop;
}


void Request::done() {
	tx -> done();
}

bool Request::empty() {
	return operations.empty();
}

/*
Request::Operation Request::createGetOp(int key) {
	Request::Operation op;
	op.type = GET;
	op.key = key;
	return op;
}

Request::Operation Request::createPutOp(int key, const std::string& value) {
	Request::Operation op;
	op.type = PUT;
	op.key = key;
	op.value = value;
	return op;
}

Request::Operation Request::createGetRangeOp(int begin, int end) {
	Request::Operation op;
	op.type = GETRANGE;
	op.begin = begin;
	op.end = end;
	return op;
}

*/
