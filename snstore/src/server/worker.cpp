#include "worker.hpp"
#include "requestQueueFactory.hpp"

Worker::Worker(int begin_, int end_, RequestQueue::QueueType type) {
	begin = begin_;
	end = end_;
	requestQueue = RequestQueueFactory::createRequestQueue(type);
	data = new std::string[end - begin];
  	thread_t = boost::thread(&Worker::processRequest, this);
}

Worker::~Worker() {
}

void Worker::pushRequest(RequestPtr r) {
	requestQueue.push(r);
}

void Worker::processRequest() {
	while (true) {
		RequestPtr curRequest;
		curRequest = requestQueue.pop();
		while (!curRequest -> empty()) {
			const Request::Operation curOp = curRequest -> popOp();
			switch (curOp.type) {
				case Request::GET:
					curRequest -> addResult(curOp.key, data[curOp.key - begin]);
//					std::cout << "add result, key: " << curOp.key << ", " << "value: " << data[curOp.key - begin] << std::endl;
					break;
				case Request::PUT:
					data[curOp.key - begin] = curOp.value;
					break;
				case Request::GETRANGE:
					for (int i = curOp.begin - begin; i <= curOp.end - begin; i++)  {
						curRequest -> addResult(i + begin, data[i]);	
//						std::cout << "add result, key: " << i + begin << ", " << "value: " << data[i] << std::endl;
					}
					break;
				default:
					break;
			}
			curRequest -> done();
		}
	}
}
/*
#include "worker.h"

void Worker::start() {
  processThread = boost::thread(&Worker::processRequests, this);
}

void Worker::processRequests() {
  while(true) {
    {
      boost::mutex::scoped_lock lock(worker_mutex);
      while(operations->empty())
        operations_con->wait(lock);

      while(!operations->empty()) {
        string operation = operations->front();
        operations->pop();        
        Debug("Worker process operations: " << operation << endl);
        istringstream iss(operation);
        vector<string> tokens;
        copy(istream_iterator<string>(iss),
             istream_iterator<string>(),
             back_inserter<vector<string> >(tokens));
        if ("get" == tokens[0]) {
          string str = operation;
          str += " " + db[string2int(tokens[1])];
          Debug("Worker get process:" << str << endl);
          results->push(str);
        } else if ("put" == tokens[0]) {
          db[string2int(tokens[1])] = tokens[2];
          // Nothing now
          string str = "put true";
          results->push(str);
        } else if ("getRange" == tokens[0]) {
          string str = operation;
          int minKey = string2int(tokens[1]);
          int maxKey = string2int(tokens[2]);
          for (int i = minKey; i <= maxKey; ++i) {
            if (db.count(i) == 0)
              str += " " + db[minKey];
            else 
              str += " ";
          }
          results->push(str);
        }
      } // end while operations->empty
      results_con->notify_all();
      Debug("Worker process end.\n");
    }
  }
}
*/
