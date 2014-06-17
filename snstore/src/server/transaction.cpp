#include "transaction.hpp"

Transaction::Transaction(int size_) {
	size = size_;
}

void Transaction::addResult(int key, std::string value) {
	boost::lock_guard<boost::mutex> guard(mutex_t);
	results[key] = value;		
	if (--size == 0)
		cond_t.notify_one();
}

const std::map<int, std::string>& Transaction::getResults() {
	return results;
}

