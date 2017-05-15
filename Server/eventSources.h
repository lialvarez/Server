#ifndef EVENTSOURCES_H
#define EVENTSOURCES_H

#include <string>
#include <vector>
#include <fstream>
#include <boost\asio.hpp>
#include "Screen.h"
#include "genericEventSource.h"
#include "Networking.h"
#include <ctime>

#define ONE_MINUTE 60

class NetworkEventSource : public genericEventSource
{
public:
	NetworkEventSource(Networking *_server) :server(_server){}
	bool isThereEvent();
	void setServerIP(std::string _serverIP);
	genericEvent* insertEvent();
	std::string getServerIP();
	Networking *server;
private:
	std::string serverIP;
};

class UserEventSource : public genericEventSource
{
public:
	UserEventSource(Screen *Terminal);
	bool isThereEvent();
	genericEvent* insertEvent();
	std::string getCommand();	//getter del commando ingresado
	Screen *terminal;
private:

	std::vector<std::string> words;	//Vector para separar los argumentos ingresados
	std::vector<char> buffer;
	std::string command;

	int inputChar;
	char ** args;

};

class TimeoutEventSource : public genericEventSource
{
public:
	TimeoutEventSource();
	bool isThereEvent();
	void startTimer();
	void stopTimer();
	genericEvent* insertEvent();
private:
	clock_t tInicial;
	bool timeout;	//Si está en true se cumplió el tiempo.
	bool timerRunning;
};

class SoftwareEventSource : public genericEventSource
{
public:
	SoftwareEventSource();
	~SoftwareEventSource();
	bool isThereEvent();
	//loadSoftwareEvent(ev); //TODO: agregar una funcion para cargar eventos de software
};

#endif // !EVENTSOURCES_H