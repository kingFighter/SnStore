#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <boost/thread.hpp>
#include <map>
#include <string>

class Transaction {
public:
	Transaction();
	void addResult(int key, int value);
	const std::map<int, int>& getResults();
	void wait();
	void done();
	void pushOp();
	
private:
	std::map<int, int> results;
	boost::mutex mutex_t;
	boost::condition_variable cond_t;
	int size;
};

#endif
