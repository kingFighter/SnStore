#include "blockRequestQueueImpl.hpp"

void BlockRequestQueueImpl::push(boost::shared_ptr<Request> r) {
	{
		boost::lock_guard<boost::mutex> guard(mutex_t);
		requestQueue.push(r);
	}
	cond_t.notify_one();
}

boost::shared_ptr<Request> BlockRequestQueueImpl::pop() {
	boost::mutex::scoped_lock lock(mutex_t);
	while (requestQueue.empty())
		cond_t.wait(lock);

	BlockRequestQueueImpl::RequestPtr result = requestQueue.front();
	requestQueue.pop();
	return result;
}
