#include "usefulInfo.h"
#include "eventGenerator.h"
#include "genericFSM.h"
#include "eventSources.h"
#include "Screen.h"
#include <string>
int main()
{
	Screen Terminal;
	UserEventSource userSource(&Terminal);
	usefulInfo Info(&userSource);
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
