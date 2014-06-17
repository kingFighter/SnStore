#ifndef REQUESTQUEUE_H
#define REQUESTQUEUE_H
#include "requestQueueImpl.hpp"
#include "request.hpp"

class RequestQueue {
public:
	typedef boost::shared_ptr<Request> RequestPtr;
	typedef boost::shared_ptr<RequestQueueImpl> RequestQueueImplPtr;
	enum QueueType {
		BLOCK = 0,
		WAIT_FREE
	};
	RequestQueue(RequestQueueImplPtr ptr);
	RequestQueue() {}
	~RequestQueue();
	void push(RequestPtr r);
	RequestPtr pop();
private:
	RequestQueueImplPtr queueImpl;
};
#endif
