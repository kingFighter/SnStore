#ifndef WFREQUESTQUEUEIMPL_H
#define WFREQUESTQUEUEIMPL_H
#include "requestQueueImpl.hpp"
#include <boost/lockfree/spsc_queue.hpp>

class WFRequestQueueImpl : public RequestQueueImpl {
public:
	typedef boost::shared_ptr<Request> RequestPtr;
	void push(RequestPtr r);
	RequestPtr pop();
	~WFRequestQueueImpl() {}


private:
	boost::lockfree::spsc_queue<RequestPtr, boost::lockfree::capacity<1024> > request_queue;	
};

#endif
