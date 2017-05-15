
#include "Networking.h"

Networking::Networking()
{
	IO_handler = new boost::asio::io_service();
	serverSocket = new boost::asio::ip::tcp::socket(*IO_handler);
	std::cout << std::endl << "Ready. Port " << CONNECTION_PORT << " created" << std::endl;
	packageArrived = false;
}


Networking::~Networking()
{
	serverSocket->close();
	delete serverSocket;
	delete IO_handler;
}


void Networking::setServerAcceptor(boost::asio::ip::tcp::acceptor* newAcceptor)
{
	serverAcceptor = newAcceptor;
}


void Networking::startConnection()
{
	std::cout << std::endl << "Start Listening on port " << CONNECTION_PORT << std::endl;
	serverAcceptor->accept(*serverSocket);
	std::cout << "Somebody connected to port " << CONNECTION_PORT << std::endl;
	serverSocket->non_blocking(true);
}


boost::asio::io_service* Networking::getIO_handler()
{
	return IO_handler;
}

unsigned int Networking::getBlockNumber()
{
	return blockNumber;
}

void Networking::packageDecode()	//agregarle WRQ Y RRQ
{
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

	//char buf[PACKAGE_MAX_SIZE] = "Hello from server."; //esto se borra

	size_t len;
	boost::system::error_code error;

	do
	{													
		len = clientSocket->write_some(boost::asio::buffer(outputPackage, packageSize), error); //declarar y hacer una funcion que ponga packageSize
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;
}

bool Networking::receivePackage()
{
	bool ret = false;
	boost::system::error_code error;
	char buf[PACKAGE_MAX_SIZE];
	size_t len = 0;
	do
	{
		len = clientSocket->read_some(boost::asio::buffer(buf), error);
	} while (error.value() == WSAEWOULDBLOCK);

	if (!error)
	{
		strcpy_s(inputPackage, len, buf);
		ret = true;
	}
	else
	{
		ret = false;
	}
	return ret;
}


MYBYTE * Networking::getInputPackage()
{
	return inputPackage;
}