#ifndef EVENTS_H
#define EVENTS_H

#include "Screen.h"
#include "genericEvent.h"

typedef char MYBYTE;

//USER EVENTS

class EV_Quit : public genericEvent
{
public:
	EV_Quit() { ev = QUIT; };
private:

};

class EV_Clear : public genericEvent
{
public:
	EV_Clear(Screen *terminal) { ev = CLEAR; this->terminal = terminal; };
	Screen *terminal;
private:

};


//NETWORK EVENTS

class EV_Data : public genericEvent
{
public:
	EV_Data() { ev = DATA; };
	//TODO: funciones de cargar el buffer y de obetener la info

private:
	unsigned int blockNumber;	//Guardar el numero de bloque aca.
	MYBYTE* dataBuffer;
};

class EV_Ack : public genericEvent
{
public:
	EV_Ack() { ev = ACK; };

private:
	unsigned int blockNumber;
};

class EV_Error : public genericEvent
{
public:
	EV_Error() { ev = ERRO; };

private:

};

class EV_WRQ : public genericEvent
{
public:
	EV_WRQ() { ev = WRQ; };

private:
	std::string fileToTransfer;
};

class EV_RRQ : public genericEvent
{
public:
	EV_RRQ() { ev = RRQ; };

private:
	std::string fileToTransfer;

};

//SOFTWARE EVENTS

class EV_ConnectionFailed : public genericEvent
{
public:
	EV_ConnectionFailed() { ev = CONNECTION_FAIL; };

private:

};

class EV_LastData : public genericEvent
{
public:
	EV_LastData() { ev = LAST_DATA; };

private:

};

//TIMEOUT EVENT

class EV_Timeout : public genericEvent
{
public:
	EV_Timeout() { ev = TIMEOUT; };

private:

};

#endif // !EVENTS_H

