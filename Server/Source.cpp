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
	Networking Server;
	NetworkEventSource networkSource(&Server);
	UserEventSource userSource(&Terminal);
	usefulInfo Info(&userSource, &networkSource);
	genericEvent *ev;
	eventGenerator evGen(&Info);
	genericFSM FSM;
		
	do 
	{
		evGen.generateEvent();
		ev = evGen.getNextEvent();
		if (ev != nullptr)
		{
			FSM.dispatch(ev);
		}
	} while (FSM.getCurrentState()->getLastEvent() != QUIT);
}
