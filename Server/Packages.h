
#ifndef PACKAGES_H
#define PACKAGES_H
#include <string>

typedef char MYBYTE;

class genericPackage
{
public:
	MYBYTE *package;
	virtual void setPackage() {}
	opCodes code;
};

class Data :public genericPackage
{
public:
	Data(std::string _data, unsigned int _blockNumber = 1) :data(_data), blockNumber(_blockNumber) { code = DATA_OP; }
	std::string data;
	unsigned int blockNumber;
	void setPackage();
};

class Acknowledge :public genericPackage
{
public:
	Acknowledge(unsigned int _blockNumber) :blockNumber(_blockNumber) { code = ACK_OP; }
	unsigned int blockNumber;
	void setPackage();
};

class Error :public genericPackage
{
public:
	Error(unsigned int _errorCode, std::string _errorMsg) :errorCode(_errorCode), errorMsg(_errorMsg) { code = ERROR_OP; }
	unsigned int errorCode;
	std::string errorMsg;
	void setPackage();
};

#endif // !PACKAGES_H
