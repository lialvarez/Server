
#include "Networking.h"

Networking::Networking()
{
	IO_handler = new boost::asio::io_service();

	serverSocket = new boost::asio::ip::tcp::socket(*IO_handler);

	std::cout << std::endl << "Ready. Port " << CONNECTION_PORT << " created" << std::endl;

	setServerAcceptor(new boost::asio::ip::tcp::acceptor(*getIO_handler(),						//ver el delete de esto
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), CONNECTION_PORT)));

	startConnection();

	std::cout << "Somebody connected to port " << CONNECTION_PORT << std::endl;

	packageArrived = false;
}

Networking::~Networking()
{
	delete IO_handler;
	delete serverSocket;
	//agregar el delete que falta
}


void Networking::setServerAcceptor(boost::asio::ip::tcp::acceptor* newAcceptor)
{
	serverAcceptor = newAcceptor;
}


void Networking::startConnection()
{
	std::cout << std::endl << "Start Listening on port " << CONNECTION_PORT << std::endl;

	serverAcceptor->accept(*serverSocket);
}


boost::asio::io_service* Networking::getIO_handler()
{
	return IO_handler;
}


void Networking::sendData(FILE *filePointer, unsigned int blockNumber)
{
	packageSET(DATA_OP, blockNumber, filePointer);
	sendPackage();
}

void Networking::sendAck(unsigned int blockNumber /*= 0*/)
{
	packageSET(ACK_OP, blockNumber);
	sendPackage();
}

void Networking::sendError(std::string errorMsg, unsigned int errorCode)
{
	this->errorCode = errorCode;
	this->errorMsg = errorMsg;
	packageSET(ERROR_OP);
	sendPackage();
}

void Networking::packageSET(opCodes opCode, unsigned int blockNumber /*= 0*/, FILE *filePointer /*= NULL*/)
{
	std::string mode("octet");
	switch (opCode)
	{
	case DATA_OP:
		char auxBuffer[512];
		unsigned int bytesToSend = fread(auxBuffer, 1, 512, filePointer);	//cuenta los bytes a enviar
		outputPackage = new _BYTE[bytesToSend + 4];
		outputPackage[0] = 0x00;
		outputPackage[1] = (_BYTE)3;
		outputPackage[2] = (blockNumber & 0xFF00) >> 8;
		outputPackage[3] = (blockNumber & 0x00FF);
		strcpy((char *)outputPackage + 4, auxBuffer);
		break;
	case ACK_OP:
		outputPackage = new _BYTE[4];
		outputPackage[0] = 0x00;
		outputPackage[1] = (_BYTE)4;
		outputPackage[2] = (blockNumber & 0xFF00) >> 8;
		outputPackage[3] = (blockNumber & 0x00FF);
		break;
	case ERROR_OP:
		outputPackage = new _BYTE[errorMsg.length() + 5];
		outputPackage[0] = 0x00;
		outputPackage[1] = (_BYTE)5;
		outputPackage[2] = 0x00;
		outputPackage[3] = (_BYTE)errorCode;
		strcpy((char *)outputPackage + 4, errorMsg.c_str());
		break;
	default:
		break;
	}
}

void Networking::sendPackage()
{
	std::cout << std::endl << "Sending package" << std::endl;

	boost::function<void(const boost::system::error_code&, std::size_t)> handler(
		boost::bind(&Networking::afterSending, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

	boost::asio::async_write(*serverSocket, boost::asio::buffer(outputPackage, PACKAGE_MAX_SIZE), handler); //ver si cambiar el 600
}

void Networking::afterSending(const boost::system::error_code& error, std::size_t transfered_bytes) {
}

void Networking::receivePackage()
{
	boost::system::error_code error;
	_BYTE emptyBuf[PACKAGE_MAX_SIZE] = { NULL };
	_BYTE buf[PACKAGE_MAX_SIZE] = { NULL };

	std::cout << "Receiving package" << std::endl;

	boost::function<void(const boost::system::error_code&, std::size_t)> handler(
		boost::bind(&Networking::afterReceiving, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

	async_read(*serverSocket, boost::asio::buffer(buf, PACKAGE_MAX_SIZE), handler);		// Si recibe algo, lo guarda en buf.

	if (strcmp(buf, emptyBuf))
	{
		packageArrived = false;
	}
	else
	{
		packageArrived = true;

		if (error != boost::asio::error::eof)											//Se guarda buf en inputPackage (el paquete recibido)
		{
			for (unsigned int i = 0; i < PACKAGE_MAX_SIZE; i++)
			{
				inputPackage[i] = buf[i];
			}
		}
		else
			std::cout << "Error while trying to connect to receive package %d" << error.message() << std::endl;
	}
}

void Networking::afterReceiving(const boost::system::error_code& error, std::size_t transfered_bytes) {
}