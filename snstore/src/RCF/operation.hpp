#ifndef OPERATION_H
#define OPERATION_H


struct Operation {
	enum Type {
		GET = 0,
		PUT,
		GETRANGE
	};
	Type type;
	int key;
	std::string value;
	int begin;
	int end;
};

static inline Operation createGetOp(int key) {
	Operation op;
	op.type = Operation::GET;
	op.key = key;
	return op;
}

static inline Operation createPutOp(int key, const std::string& value) {
	Operation op;
	op.type = Operation::PUT;
	op.key = key;
	op.value = value;
	return op;
}

static inline Operation createGetRangeOp(int begin, int end) {
	Operation op;
	op.type = Operation::GETRANGE;
	op.begin = begin;
	op.end = end;
	return op;
}

template<typename Archive>
void serialize(Archive &ar, Operation &o)
{
	ar & o.type & o.key & o.value & o.begin & o.end;
}

#endif
