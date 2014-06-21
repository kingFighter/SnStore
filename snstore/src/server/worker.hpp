#ifndef WORKER_H
#define WORKER_H
#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include "request.hpp"
#include "requestQueue.hpp"

class Worker {
public:
	typedef boost::shared_ptr<Request> RequestPtr;
	Worker(int begin_, int end_, RequestQueue::QueueType type);
	~Worker();
	void pushRequest(RequestPtr r);
private:
	void processRequest();
	int begin;
	int end;
	boost::thread thread_t;
	RequestQueue requestQueue;
	int* data;
};

#endif
