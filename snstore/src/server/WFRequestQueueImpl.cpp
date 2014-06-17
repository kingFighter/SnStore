#include "WFRequestQueueImpl.hpp"

void WFRequestQueueImpl::push(boost::shared_ptr<Request> r) {
	while (!request_queue.push(r));
}

boost::shared_ptr<Request> WFRequestQueueImpl::pop() {
	boost::shared_ptr<Request> result;
	while (!request_queue.pop(result));
	return result;
}
