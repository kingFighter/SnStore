#ifndef RCF_INTERFACE
#define RCF_INTERFACE

#include <map>
#include <vector>
#include <string>

#include "operation.hpp"

#include <RCF/RCF.hpp>
#include <RCF/Idl.hpp>
#include <boost/serialization/map.hpp>
#include <SF/map.hpp>
#include <SF/vector.hpp>
#include <SF/string.hpp>

RCF_BEGIN(I_Put, "I_Put")
	RCF_METHOD_R2(int, put, int, std::string);
RCF_END(I_Put);

RCF_BEGIN(I_Get, "I_Get")
	RCF_METHOD_R1(std::string, get, int);
RCF_END(I_Get);

RCF_BEGIN(I_GetRange, "I_GetRange")
	RCF_METHOD_R3(bool, getRange, int, int, std::map<int, int>);// std::map<int, std::string> &);
//	RCF_METHOD_R2(bool, getRange, int, int);
RCF_END(I_GetRange);

RCF_BEGIN(I_ExecTx, "I_ExecTx")
	RCF_METHOD_R2(bool, execTx, std::vector<Operation>, std::map<int, std::string> &);
//	RCF_METHOD_R1(bool, execTx, std::vector<Operation>);
RCF_END(I_ExecTx);

#endif
