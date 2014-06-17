#ifndef REQUESTQUEUEIMPL_H
#define REQUESTQUEUEIMPL_H
#include "request.hpp"

class RequestQueueImpl {
public:
	virtual void push(boost::shared_ptr<Request> r) = 0;
	virtual boost::shared_ptr<Request> pop() = 0;
	virtual ~RequestQueueImpl() {}
};

#endif
