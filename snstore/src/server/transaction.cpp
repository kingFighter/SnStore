#include "transaction.hpp"

Transaction::Transaction() {
	size = 0;
}

void Transaction::pushOp() {
	size++;
}

void Transaction::addResult(int key, std::string value) {
	boost::lock_guard<boost::mutex> guard(mutex_t);
	results[key] = value;		
}

void Transaction::done() {
	if (--size == 0)
		cond_t.notify_one();
}

const std::map<int, std::string>& Transaction::getResults() {
	return results;
}

void Transaction::wait() {
	boost::mutex::scoped_lock lock(mutex_t);
//	std::cout << "wait, size: " << size << std::endl;
	if (size != 0)
		cond_t.wait(lock);
}
