
/////////////////////////////////////////////////////////////////////////////////////////////	
//
// Instituto Tecnologico de Buenos Aires						18/05/2017
//
// Algoritmos y Estructuras de Datos - Trabajo Práctico 7
//
//
// Alvarez, Lisandro						 ////////////////////////////////////////////////
// Stewart, María Luz						//	   Networking + Event Driven Programming
// Muller, Malena						   //////////////////////////////////////////////////
//
// SERVER
//
// Este programa comienza aguardando la conexion de un cliente y recibe una solicitud de
// dicho cliente para enviarle un archivo o solicitarle un archivo. Se implemento el 
// protocolo TFTP, cuyas especificaciones se encuentran en la documentacion de RFC1350.
// https://tools.ietf.org/html/rfc1350
//
// Ingresar "help" para información sobre la interaccion del usuario con el programa.
//
/////////////////////////////////////////////////////////////////////////////////////////////


#include "usefulInfo.h"
#include "eventGenerator.h"
#include "genericFSM.h"
#include "eventSources.h"
#include "Screen.h"
#include "Networking.h"
#include <string>
#include <sstream>

int main()
{
	Screen Terminal;
	FileSystem fileSystem;
	Networking Server;
	TimeoutEventSource Timeout;
	SoftwareEventSource Software;
	NetworkEventSource networkSource(&Server);
	UserEventSource userSource(&Terminal);
	usefulInfo Info(&userSource, &Timeout, &networkSource, &fileSystem, &Software);
	genericEvent *ev;
	eventGenerator evGen(&Info);
	genericFSM FSM;
	Terminal.putClear("Listening on port 69...");
	Server.startConnection();
	Terminal.putNext("Connection established");
		
	do 
	{
		evGen.generateEvent();
		ev = evGen.getNextEvent();
		if (ev != nullptr)
		{
			FSM.dispatch(ev, &Info);
		}

	} while (FSM.getCurrentState()->getLastEvent() != QUIT);
}
