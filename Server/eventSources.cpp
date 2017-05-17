#include "eventSources.h"
#include "Screen.h"
#include "Events.h"
#include <boost\algorithm\string\classification.hpp>
#include <boost\algorithm\string\split.hpp>
#include <boost\date_time\posix_time\posix_time.hpp>

/*****  NETWORK EVENT SOURCE  *****/
NetworkEventSource::NetworkEventSource(Networking *_networkInterface) :networkInterface(_networkInterface) 
{}

bool NetworkEventSource::isThereEvent()
{ 
	unsigned char blockLow, blockHigh;
	bool ret = false;
	std::ifstream fileStream;
	std::vector<char> aux;


	if (networkInterface->receivePackage())	//verifica si se recibio algo
	{
		switch (networkInterface->getInputPackage()[1])	//segun el tipo de paquete devuelvo el tipo de evento
		{
		case WRQ_OP:
			fileRequested = std::string(&networkInterface->getInputPackage()[2]);
			fileStream.open(fileRequested);
			if (fileStream.is_open())	//Si el archivo YA EXISTE, invalido la solicitud
			{
				ret = true;
				evCode = ERRO;
				errorCode = FILE_ALREADY_EXISTS;
				fileStream.close();
			}
			else	//Si el archivo no existe, valido la solicitud
			{
				ret = true;
				evCode = WRQ;
			}
			break;
		case RRQ_OP:
			fileRequested = std::string(&networkInterface->getInputPackage()[2]);
			fileStream.open(fileRequested);
			if (fileStream.is_open())	//Si el archivo existe, valido la solicitud
			{
				ret = true;
				evCode = RRQ;
				fileStream.close();
			}
			else
			{
				ret = true;
				evCode = ERRO;
				errorCode = FILE_NOT_FOUND;
			}
			break;
		case DATA_OP:
			aux = std::vector<char>(networkInterface->getInputPackage());
			data = std::vector<char>(aux.begin() + 4, aux.end());
			
			blockLow = networkInterface->getInputPackage()[3];
			blockHigh = networkInterface->getInputPackage()[2];
			
			blockNumber = blockHigh;
			blockNumber = (blockNumber << 8) + blockLow;


			if (blockNumber != expectedBlockNum)
			{
				ret = true;
				evCode = ERRO;
			}
			else
			{
				ret = true;
				if (data.size() < 512)
				{
					evCode = LAST_DATA;
				}
				else
				{
					evCode = DATA;
				}
			}
			break;
		case ACK_OP:
			blockLow = networkInterface->getInputPackage()[3];
			blockHigh = networkInterface->getInputPackage()[2];

			blockNumber = blockHigh;
			blockNumber = (blockNumber << 8) + blockLow;
		
			if (blockNumber != expectedBlockNum)
			{
				ret = true;
				evCode = ERRO;
			}
			else
			{
				ret = true;
				evCode = ACK;
			}
			break;
		case ERROR_OP:
			errorCode = (errorCodes)(networkInterface->getInputPackage()[2]);
			errorMsg = std::string(networkInterface->getInputPackage().begin() + 4, networkInterface->getInputPackage().end());
			ret = true;
			evCode = ERRO;
			break;
		default:
			break;
		}
	}
	return ret;
}

genericEvent * NetworkEventSource::insertEvent()
{
	genericEvent * ret;
	switch (evCode)
	{
	case RRQ:
		ret = (genericEvent *) new EV_RRQ();
		break;
	case WRQ:
		ret = (genericEvent *) new EV_WRQ();
		break;
	case DATA:
		ret = (genericEvent *) new EV_Data();
		break;
	case LAST_DATA:
		ret = (genericEvent *) new EV_LastData();
		break;
	case ACK:
		ret = (genericEvent *) new EV_Ack();
		break;
	case ERRO:
		ret = (genericEvent *) new EV_Error();
		break;
	default:
		break;
	}
	return ret;
}

/*****  USER EVENT SOURCE  *****/


UserEventSource::UserEventSource(Screen *terminal)
{
	this->terminal = terminal;
	this->terminal->initTerminal();	//Inicializa la terminal por donde interactua el usuario
}

std::string UserEventSource::getCommand()
{
	return command;
}

std::string UserEventSource::getFileToTransfer()
{
	return fileToTransfer;	//Devuelve el nombre del archivo a transferir
}

bool UserEventSource::isThereEvent()
{
	std::fstream auxFile;
	bool ret = false;
	inputChar = getch();

	switch (inputChar)
	{
	case ERR:

		ret = false;	//Si no se ingresa tecla, indica que no hay evento
		break;			//y no carga caracter en buffer.

	case BACKSPACE:

		//Si se apreta "backspace" borra y quita caracteres del buffer
		//hasta que llega el comienzo de la linea de comando
		//Tambien elimina los caracteres ingresados de la teminal
		if (buffer.size() > 0)
		{
			buffer.pop_back();
			delch();
		}
		if (terminal->terminalWindow->_curx == 6)	//Posicion donde arranca la linea de comando
		{
			terminal->terminalWindow->_curx++;	//Para que el cursor no pse vaya del area permitida
		}
		ret = false;	//No hay evento
		break;

	case ENTER:

		command = std::string(buffer.begin(), buffer.end());	//Almaceno la linea de comando ingresada en command
		std::transform(command.begin(), command.end(), command.begin(), tolower);	//
		boost::split(words, command, boost::is_any_of(", "), boost::token_compress_on);	//Se separan las palabras ingresadas

		if (words.size() == 1)
		{
			if (strcmp(words[0].c_str(), "quit") == 0)
			{
				evCode = QUIT;
				ret = true;
			}
			else if (strcmp(words[0].c_str(), "clear") == 0)
			{
				evCode = CLEAR;
				ret = true;
			}
		}
		terminal->setCommandLine();
		buffer.clear();
		words.clear();
		break;

	default:
		//Si el caracter ingresado es un caracter valido(ningun caracter de control)
		//lo añade al buffer
		if (inputChar > ASCII_START && inputChar < ASCII_END)
		{
			buffer.push_back(inputChar);
			ret = false;
		}
		break;
	}
	return ret;
}

genericEvent * UserEventSource::insertEvent()
{
	genericEvent * ret;

	switch (evCode)
	{

	case CLEAR:
		ret = (genericEvent *) new EV_Clear();
		break;
	case QUIT:
		ret = (genericEvent *) new EV_Quit;
		break;
	default:
		break;
	}
	return ret;
}

/*****  TIMEOUTS EVENT SOURCE  *****/

TimeoutEventSource::TimeoutEventSource()
{
	timeout = false;
	timerRunning = false;
	timeoutsCount = 0;
}

bool TimeoutEventSource::isThereEvent()
{
	if (((clock() - tInicial) > ONE_MINUTE * CLOCKS_PER_SEC) && timerRunning)
	{
		timeout = true;
		timerRunning = false;
		timeoutsCount++;
		if (timeoutsCount == MAX_TIMEOUTS)
		{
			evCode = CONNECTION_FAIL;
		}
		else
		{
			evCode = TIMEOUT;
		}
	}
	else
	{
		timeout = false;
		evCode = NO_EV;
	}
	return timeout;
}

void TimeoutEventSource::startTimer()

{
	timeout = false;	//Se setea la variable de control en false, indicando que no ha ocurrido timeout
	tInicial = clock();
	timerRunning = true;
	timeoutsCount = 0;
}

void TimeoutEventSource::stopTimer()
{
	timerRunning = false;
}

genericEvent * TimeoutEventSource::insertEvent()
{
	genericEvent * ret;

	switch (evCode)
	{
	case TIMEOUT:
		ret = (genericEvent *) new EV_Timeout;
		break;
	case CONNECTION_FAIL:
		ret = (genericEvent *) new EV_ConnectionFailed;
		break;
	default:
		break;
	}
	return ret;
}

/*****  SOFTWARE EVENT SOURCE   *****/

SoftwareEventSource::SoftwareEventSource() :ev(false) {};

bool SoftwareEventSource::isThereEvent()
{
	bool ret = false;
	if (fileInterface->lastData && !ev)
	{
		evCode = LAST_DATA;
		ev = true;
		ret = true;
	}
	return ret;
};

genericEvent* SoftwareEventSource::insertEvent()
{
	genericEvent *ret = nullptr;
	if (evCode == LAST_DATA)
	{
		ret = (genericEvent *) new EV_LastData();
	}
	return ret;
}