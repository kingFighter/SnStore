#ifndef WORKER_H
#define WORKER_H
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include "request.hpp"
#include "transaction.hpp"

class Worker {
public:
	Worker();
	~Worker();
	void pushRequest(Request r);
private:
	boost::thread thread_t;
	boost::lock_free::queue request_queue;		
	string* data;	
}
