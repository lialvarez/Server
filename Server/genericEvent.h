#ifndef GENERICEVENT_H
#define GENERICEVENT_H

#include <string>

typedef enum { NO_EV, QUIT, CLEAR, FILE_ERROR, ERRO, DATA, ACK, WRQ, RRQ, LAST_DATA, TIMEOUT, CONNECTION_FAIL } eventCode;
//Atencion que el evento ERROR es ERRO porque ERROR ya esta definido

class genericEvent
{
public:
	genericEvent() { ev = NO_EV; }//por default, se setea el tipo de evento en NO_EV. Si otra clase hereda genericEvent, puede cambiar el constructor para setear el evento como el que corresponda
	eventCode getEventType() { return ev; }	//getter del tipo de evento

protected:
	std::string selectedFile;
	eventCode ev;
};


#endif // !GENERICEVENT_H