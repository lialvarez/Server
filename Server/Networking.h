#ifndef NETWORKING_H
#define NETWORKING_H

#include <iostream>
#include <string>
#include <exception>
#include <array>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <Windows.h> //ver si sacar
#include <iostream>

#define CONNECTION_PORT 69		//puerto TFTP
#define PACKAGE_MAX_SIZE 516	


typedef char MYBYTE;

typedef enum {DATA_OP, ACK_OP, ERROR_OP}opCodes;

class Networking
{
public:
	void sendPackage(genericPackage *Pkg);
	void receivePackage();
	Networking();//
	~Networking(); //Ver cuando llamarlo (cuando hay error o termina la comunicacion)
	void startConnection();//

	unsigned int getBlockNumber();
	MYBYTE *getInputPackage();
	errorCodes getErrorCode();
	std::string getData();
	std::string getErrorMsg();
	unsigned int getBlockNumber();

private:
	boost::asio::io_service *IO_handler;
	boost::asio::ip::tcp::socket *serverSocket;
	boost::asio::ip::tcp::acceptor *serverAcceptor;
	boost::asio::ip::tcp::resolver::iterator endpoint;

	void packageDecode();

	std::string fileToTransfer;

	opCodes receivedPackageType;
	std::string data;	//Se almacena la data en caso de recibir DATA
	std::string errorMsg;
	errorCodes errorCode;
	unsigned int blockNumber;

	MYBYTE *inputPackage;
	MYBYTE *outputPackage;

	const char* serverAddress;

	MYBYTE *inputPackage;	//ver si quedan
	MYBYTE *outputPackage;
	bool packageArrived;
};
#endif // !NETWORKING_H
