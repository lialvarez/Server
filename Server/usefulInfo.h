#ifndef USEFULINFO_H
#define USEFULINFO_H

#include <string>
#include "eventSources.h"
#include "FileSystem.h"
class usefulInfo
{
public:
	//Hay que agrgarle los punteros a las otras fuentes al constructor
	//y pasar el constructor a un .cpp
	usefulInfo(UserEventSource *UsEvSrc, TimeoutEventSource *TOEvSrc, NetworkEventSource *NETEvSrc) 
		:userSrc(UsEvSrc), timeoutSrc(TOEvSrc), networkSrc(NETEvSrc){
		networkInterface = networkSrc->networkInterface;
		userInterface = userSrc->terminal;
	}

	NetworkEventSource* networkSrc;	//TODO: ponerle getters quizas? quedaria mas prolijo o es al pedo??
	UserEventSource* userSrc;
	TimeoutEventSource* timeoutSrc;
	SoftwareEventSource* softwareSrc;

	Networking *networkInterface;
	Screen  *userInterface;
	FileSystem *fileInterface;
	
	genericPackage *nextPkg;
private:

};

#endif // !USEFULINFO_H