#include "eventSources.h"
#include "Screen.h"
#include "Events.h"
#include <boost\algorithm\string\classification.hpp>
#include <boost\algorithm\string\split.hpp>
#include <boost\date_time\posix_time\posix_time.hpp>

/*****  NETWORK EVENT SOURCE  *****/

NetworkEventSource::NetworkEventSource() {};
NetworkEventSource::~NetworkEventSource() {};

bool NetworkEventSource::isThereEvent() //MALE: esta es la funcion que lee lo que le envian por red
{ 
	server.receivePackage(); //en el main llamar al constructor de Networking, creando el objeto server
	return server.getPackageArrived();
} 


void NetworkEventSource::setServerIP(std::string _serverIP)
{
	serverIP = _serverIP;
}

std::string NetworkEventSource::getServerIP()
{
	return serverIP;
}

/*****  USER EVENT SOURCE  *****/

//NO TOCAR LOS DE USER, CREO QUE YA ESTAN LISTOS

UserEventSource::UserEventSource(Screen *terminal)
{
	this->terminal = terminal;
	this->terminal->initTerminal();	//Inicializa la terminal por donde interactua el usuario
}

std::string UserEventSource::getCommand()
{
	return command;
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
		ret = (genericEvent *) new EV_Clear(terminal);
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
}

bool TimeoutEventSource::isThereEvent()
{
	if (((clock() - tInicial) > ONE_MINUTE * CLOCKS_PER_SEC) && timerRunning)
	{
		timeout = true;
		timerRunning = false;
		evCode = TIMEOUT;
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
}

genericEvent * TimeoutEventSource::insertEvent()
{
	genericEvent * ret;

	switch (evCode)
	{
	case TIMEOUT:
		ret = (genericEvent *) new EV_Timeout;
		break;
	default:
		break;
	}
	return ret;
}

/*****  SOFTWARE EVENT SOURCE   *****/

SoftwareEventSource::SoftwareEventSource() {};
SoftwareEventSource::~SoftwareEventSource() {}
bool SoftwareEventSource::isThereEvent() { return false; };