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
