#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <boost/thread.hpp>
#include <map>
#include <string>

class Transaction {
public:
	Transaction(int size_);
	void addResult(int key, std::string value);
	const std::map<int, std::string>& getResults();
	
private:
	std::map<int, std::string> results;
	boost::mutex mutex_t;
	boost::condition_variable cond_t;
	int size;
};

#endif
