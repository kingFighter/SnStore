#ifndef WORKER_H
#define WORKER_H
#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include "request.hpp"

class Worker {
public:
	Worker();
	~Worker();
	void pushRequest(Request& r);
private:
	boost::thread thread_t;
	boost::lockfree::spsc_queue<int, boost::lockfree::capacity<1024> > request_queue;		
	std::string* data;
};

#endif
