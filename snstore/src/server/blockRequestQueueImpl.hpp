#ifndef BLOCKINGREQUESTQUEUE_H
#define BLOCKINGREQUESTQUEUE_H
#include "requestQueueImpl.hpp"
#include <queue>
#include <boost/thread/thread.hpp>

class BlockRequestQueueImpl : public RequestQueueImpl {
public:
	typedef boost::shared_ptr<Request> RequestPtr;
	void push(RequestPtr r);
	RequestPtr pop();
	~BlockRequestQueueImpl() {}
	
private:
	boost::mutex mutex_t;
	boost::condition_variable cond_t;	
	std::queue<RequestPtr> requestQueue;
};

#endif

