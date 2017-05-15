#include "Networking.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <conio.h>


Networking::Networking()
{
	IO_handler = new boost::asio::io_service();
	serverSocket = new boost::asio::ip::tcp::socket(*IO_handler);
	serverAcceptor = new boost::asio::ip::tcp::acceptor(*IO_handler,
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), CONNECTION_PORT));
}

Networking::~Networking()
{
	serverAcceptor->close();
	serverSocket->close();
	delete serverSocket;
	delete IO_handler;
}

void Networking::startConnection()
{
	serverAcceptor->accept(*serverSocket);
	serverSocket->non_blocking(true);
}

unsigned int Networking::getBlockNumber()
{
	return blockNumber;
}

void Networking::packageDecode()
{
	std::string receivedFile;
	receivedPackageType = (opCodes)inputPackage[1];
	switch (receivedPackageType)
	{
	case DATA_OP:
		blockNumber = (inputPackage[2] << 8) + inputPackage[3];
		data = inputPackage[4];
		break;
	case ACK_OP:
		blockNumber = (inputPackage[2] << 8) + inputPackage[3];
		break;
	case ERROR_OP:
		errorCode = (errorCodes)inputPackage[3];
		errorMsg = inputPackage[4];
		break;
	default:
		break;
	}
}

errorCodes Networking::getErrorCode()
{
	return errorCode;
}

std::string Networking::getErrorMsg()
{
	return errorMsg;
}

std::string Networking::getData()
{
	return data;
}

void Networking::sendPackage(genericPackage *Pkg)
{
	Pkg->setPackage();

	size_t len;
	boost::system::error_code error;

	do
	{													
		len = serverSocket->write_some(boost::asio::buffer(Pkg->package, Pkg->package.size()), error); 
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
	{ 
		/*std::cout << "Error while trying to connect to server " << error.message() << std::endl;*/
	}
		
}

bool Networking::receivePackage()
{
	bool ret = false;
	boost::system::error_code error;
	char buf[PACKAGE_MAX_SIZE];
	size_t len = 0;
	do
	{
		len = serverSocket->read_some(boost::asio::buffer(buf), error);
	} while (error.value() == WSAEWOULDBLOCK);

	if (!error)
	{
		//strcpy_s(&inputPackage[0], len, buf);
		inputPackage.clear();
		inputPackage.insert(inputPackage.end(), buf, buf + len);
		ret = true;
	}
	else
	{
		ret = false;
	}
	return ret;
}

std::vector<char> Networking::getInputPackage()
{
	return inputPackage;
}
