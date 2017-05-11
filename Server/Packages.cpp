#include "Packages.h"
#include "Networking.h"
#include <fstream>


void Acknowledge::setPackage()
{
	package = new MYBYTE[4];
	package[0] = 0x00;
	package[1] = (MYBYTE)code;
	package[2] = (blockNumber & 0xFF00) >> 8;
	package[3] = (blockNumber & 0x00FF);
}

void Data::setPackage()
{
	package = new MYBYTE[data.length() + 4];
	package[0] = 0x00;
	package[1] = (MYBYTE)code;
	package[2] = (blockNumber & 0xFF00) >> 8;
	package[3] = (blockNumber & 0x00FF);
	strcpy_s((char *)package[4], data.length(), data.c_str); //No le debe poner el /0
}

void Error::setPackage()
{
	package = new MYBYTE[errorMsg.length() + 1];
	package[0] = 0x00;
	package[1] = (MYBYTE)ERROR_OP;
	package[2] = 0x00;
	package[3] = (MYBYTE)errorCode;
	strcpy((char *)package[4], errorMsg.c_str());
}