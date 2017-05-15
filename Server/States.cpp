#include "States.h"
#include "Screen.h"
#include "Events.h"
#include <fstream>
#include <sstream>

//ST_Idle

genericState* ST_Idle::on_WRQ(genericEvent *ev, usefulInfo * Info)
{
	genericState *ret = (genericState *) new ST_ReceiveFirstData();

	std::string auxText = "File name: " + Info->networkInterface->getRequestedFile();
	Info->userInterface->putClear("Write Request received");
	Info->userInterface->putNext(auxText);

	Info->fileInterface->openFile(Info->networkInterface->getRequestedFile(), WRITE);	//Abro el archivo en modo ESCRITURA
	Info->networkSrc->expectedBlockNum = 0;
	Info->nextPkg = new Acknowledge(Info->networkSrc->expectedBlockNum);
	Info->networkInterface->sendPackage(Info->nextPkg);	//Envio el primer ACK
	Info->timeoutSrc->startTimer();	//Inicio el timer
	return ret;
}

genericState* ST_Idle::on_RRQ(genericEvent *ev, usefulInfo * Info)
{
	genericState *ret = (genericState *) new ST_ReceiveFirstAck();

	std::string auxText = "Requested file: " + Info->networkInterface->getRequestedFile();
	Info->userInterface->putClear("Read request received");
	Info->userInterface->putNext(auxText);

	Info->fileInterface->openFile(Info->networkInterface->getRequestedFile(), READ);	//Abro el archivo en modo LECTURA
	Info->networkSrc->expectedBlockNum = 1;
	Info->nextPkg = new Data(Info->fileInterface->readData(), Info->networkSrc->expectedBlockNum);
	Info->networkInterface->sendPackage(Info->nextPkg);	//Envio el primer paquete DATA
	Info->timeoutSrc->startTimer();
}

genericState *ST_Idle::on_CloseServer(genericEvent* ev, usefulInfo * Info)
{
	genericState *ret = (genericState*) new ST_Idle();
	ret->setLastEvent(QUIT);
	return ret;
}

genericState *ST_Idle::on_ClearTerminal(genericEvent *ev, usefulInfo * Info)
{
	Info->userInterface->putClear("\n");
	genericState *ret = (genericState*) new ST_Idle();
	return ret;
}

genericState *ST_Idle::on_Error(genericEvent *ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Error occurred");
	std::string auxText = "Error code: " + std::to_string(Info->networkInterface->getErrorCode());
	Info->userInterface->putNext(auxText);
	genericState *ret = (genericState *) new ST_Idle();
	ret->setLastEvent(ERRO);
	return ret;
}


//ST_ReceiveFirstAck
genericState * ST_ReceiveFirstAck::on_Ack(genericEvent * ev, usefulInfo * Info)
{
	std::string auxText = "Ackowledge #" + std::to_string(Info->networkInterface->getBlockNumber()) + " received.";
	Info->userInterface->putNext(auxText);
	Info->networkSrc->expectedBlockNum++;
	delete Info->nextPkg;
	Info->nextPkg = new Data(Info->fileInterface->readData, Info->networkSrc->expectedBlockNum);
	Info->networkInterface->sendPackage(Info->nextPkg);
	genericState *ret = (genericState*) new ST_ReceiveAck();

}

genericState * ST_ReceiveFirstAck::on_Error(genericEvent * ev, usefulInfo * Info)
{
	return (genericState*) new ST_Idle;
}

genericState * ST_ReceiveFirstAck::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	//sendWRQ(fileToTransfer);	//TODO
	return nullptr;
}

genericState * ST_ReceiveFirstAck::on_ConnectionFailed(genericEvent * ev, usefulInfo * Info)
{
	//y aca qe verga hacemo? //TODO
	//yo diria que deberiamos salir a reintentar conectar
	return nullptr;
}


//ST_ReceiveAck
genericState * ST_ReceiveAck::on_Ack(genericEvent * ev, usefulInfo * Info)
{
	//dataReceived();	//TODO	recien cuando llega el ack de la data enviada se mueve el FILE* para apuntar ala siguiente data
	//if(sendData())	//TODO
	//	return (genericState*) new ST_ReceiveLastData;
	return nullptr;
}

genericState * ST_ReceiveAck::on_Error(genericEvent * ev, usefulInfo * Info)
{
	return (genericState*) new ST_Idle;
}

genericState * ST_ReceiveAck::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	//sendData();	//TODO. Como no se invoca a la funcion dataReceived, se sigue apuntando al mismo paquete que antes
	return nullptr;
}

genericState * ST_ReceiveAck::on_ConnectionFailed(genericEvent * ev, usefulInfo * Info)
{
	return nullptr;
}

genericState * ST_ReceiveAck::on_LastData(genericEvent * ev, usefulInfo * Info)
{
	return (genericState*) new ST_ReceiveLastAck;
}

//ST_ReceiveLastAck
genericState * ST_ReceiveLastAck::on_Ack(genericEvent * ev, usefulInfo * Info)
{
	//TODO: mostrar que es ya se termino de enviar el mensaje
	return (genericState*) new ST_Idle;
}

genericState * ST_ReceiveLastAck::on_Error(genericEvent * ev, usefulInfo * Info)
{
	//TODO: mostrar error
	return (genericState*) new ST_Idle;
}

genericState * ST_ReceiveLastAck::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	//sendData()	//TODO
	return nullptr;
}

genericState * ST_ReceiveLastAck::on_ConnectionFailed(genericEvent * ev, usefulInfo * Info)
{
	return nullptr;
}

//ST_ReceiveFirstData
genericState * ST_ReceiveFirstData::on_Data(genericEvent * ev, usefulInfo * Info)
{
	//sendAck();
	return (genericState*) new ST_ReceiveData;
}

genericState * ST_ReceiveFirstData::on_Error(genericEvent * ev, usefulInfo * Info)
{
	return (genericState*) new ST_Idle;
}

genericState * ST_ReceiveFirstData::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	//sendRRQ()	//TODO
	return nullptr;
}

genericState * ST_ReceiveFirstData::on_ConnectionFailed(genericEvent * ev, usefulInfo * Info)
{
	return nullptr;
}

//ST_ReceiveData
genericState * ST_ReceiveData::on_Data(genericEvent * ev, usefulInfo * Info)
{
	//sendAck() //TODO
	return nullptr;
}

genericState * ST_ReceiveData::on_Error(genericEvent * ev, usefulInfo * Info)
{
	return (genericState*) new ST_Idle;
}

genericState * ST_ReceiveData::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	//sendAck() //TODO
	return nullptr;
}

genericState * ST_ReceiveData::on_ConnectionFailed(genericEvent * ev, usefulInfo * Info)
{
	return nullptr;
}

genericState * ST_ReceiveData::on_LastData(genericEvent * ev, usefulInfo * Info)
{
	return (genericState*) new ST_Idle;
}