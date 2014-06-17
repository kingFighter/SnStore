#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <boost/thread.hpp>
#include <map>
#include <string>

class Transaction {
public:
	Transaction();
	Transaction(Transaction& t);
	std::map<int, std::string> results;
private:
	boost::mutex mutex_t;
};

#endif
