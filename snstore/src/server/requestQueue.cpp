#include "requestQueue.hpp"

RequestQueue::RequestQueue(RequestQueueImplPtr ptr) : queueImpl(ptr){
}

RequestQueue::~RequestQueue() {
}

void RequestQueue::push(RequestQueue::RequestPtr r) {
	queueImpl -> push(r);	
}

RequestQueue::RequestPtr RequestQueue::pop() {
	return queueImpl -> pop();	
}
