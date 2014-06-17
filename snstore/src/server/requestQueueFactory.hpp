#ifndef REQUESTQUEUEFACTORY_H
#define REQUESTQUEUEFACTORY_H
#include "requestQueue.hpp"

class RequestQueueFactory {
public:
	static RequestQueue createRequestQueue(RequestQueue::QueueType type);
};

#endif
