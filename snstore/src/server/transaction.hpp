#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <boost/thread.hpp>
#include <map>
#include <string>

class Transaction {
public:
	Transaction();
	Transaction(Transaction& t);
private:
//	std::map<int, std::string> results;
//	boost::mutex mutex_t;
};

#endif
