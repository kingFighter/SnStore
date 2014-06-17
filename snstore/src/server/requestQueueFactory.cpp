#include "requestQueueFactory.hpp"
#include "blockRequestQueueImpl.hpp"
#include "WFRequestQueueImpl.hpp"
#include "requestQueue.hpp"
#include "assert.h"

RequestQueue RequestQueueFactory::createRequestQueue(RequestQueue::QueueType type) {
	switch (type) {
		case RequestQueue::BLOCK:
			return RequestQueue(boost::shared_ptr<BlockRequestQueueImpl>(new BlockRequestQueueImpl()));
			break;
		case RequestQueue::WAIT_FREE:
			return RequestQueue(boost::shared_ptr<WFRequestQueueImpl>(new WFRequestQueueImpl()));
			break;
		default:
			assert(false);	
			return RequestQueue();
			break;
	}
}
