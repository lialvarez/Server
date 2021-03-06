#include "States.h"
#include "Screen.h"
#include "Events.h"
#include <fstream>
#include <sstream>

//ST_Idle

genericState* ST_Idle::on_WRQ(genericEvent *ev, usefulInfo * Info)
{
	genericState *ret = (genericState *) new ST_ReceiveFirstData();

	std::string auxText = "File name: " + Info->networkSrc->fileRequested;
	Info->userInterface->putClear("Write Request recibido");
	Info->userInterface->putNext(auxText);

	Info->fileInterface->openFile(Info->networkSrc->fileRequested, WRITE);	//Abro el archivo en modo ESCRITURA
	Info->networkSrc->expectedBlockNum = 0;
	Info->nextPkg = new Acknowledge(Info->networkSrc->expectedBlockNum);
	Info->networkInterface->sendPackage(Info->nextPkg);	//Envio el primer ACK

	Info->timeoutSrc->startTimer();	//Inicio el timer

	Info->networkSrc->expectedBlockNum++;

	return ret;
}

genericState* ST_Idle::on_RRQ(genericEvent *ev, usefulInfo * Info)
{
	genericState *ret = (genericState *) new ST_ReceiveFirstAck();

	std::string auxText = "Archivo solicitado: " + Info->networkSrc->fileRequested;
	Info->userInterface->putClear("Read request recibido");
	Info->userInterface->putNext(auxText);

	Info->fileInterface->openFile(Info->networkSrc->fileRequested, READ);	//Abro el archivo en modo LECTURA
	Info->networkSrc->expectedBlockNum = 1;
	Info->nextPkg = new Data(Info->fileInterface->readData(), Info->networkSrc->expectedBlockNum);
	Info->networkInterface->sendPackage(Info->nextPkg);	//Envio el primer paquete DATA

	auxText = "Data block #" + std::to_string(Info->networkSrc->expectedBlockNum) + " enviado";
	Info->userInterface->putNext(auxText);

	Info->timeoutSrc->startTimer();

	return ret;
}

genericState *ST_Idle::on_CloseServer(genericEvent* ev, usefulInfo * Info)
{
	setLastEvent(QUIT);
	return nullptr;
}

genericState *ST_Idle::on_ClearTerminal(genericEvent *ev, usefulInfo * Info)
{
	Info->userInterface->initTerminal();
	genericState *ret = (genericState*) new ST_Idle();
	return ret;
}

genericState *ST_Idle::on_Error(genericEvent *ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Error occurred");
	Info->userInterface->showErrorMsg(Info->networkSrc->errorCode, Info->networkSrc->errorMsg);
	Info->nextPkg = new Error(Info->networkSrc->errorCode, Info->networkSrc->errorMsg);
	Info->networkInterface->sendPackage(Info->nextPkg);
	genericState *ret = (genericState *) new ST_Idle();
	ret->setLastEvent(ERRO);
	return ret;
}


//ST_ReceiveFirstAck

genericState * ST_ReceiveFirstAck::on_CloseServer(genericEvent *ev, usefulInfo * Info)
{
	delete Info->nextPkg;
	Info->fileInterface->closeFile();
	setLastEvent(QUIT);
	return nullptr;
}

genericState * ST_ReceiveFirstAck::on_LastData(genericEvent * ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Last data package enviado");
	Info->timeoutSrc->startTimer();
	genericState *ret = (genericState*) new ST_ReceiveLastAck;
	return ret;
}

genericState * ST_ReceiveFirstAck::on_Ack(genericEvent * ev, usefulInfo * Info)
{
	std::string auxText = "Ackowledge #" + std::to_string(Info->networkSrc->blockNumber) + " recibido.";
	Info->userInterface->putNext(auxText);

	
	delete Info->nextPkg;
	Info->networkSrc->expectedBlockNum++;
	Info->nextPkg = new Data(Info->fileInterface->readData(), Info->networkSrc->expectedBlockNum);
	Info->networkInterface->sendPackage(Info->nextPkg);

	auxText = "Data block #" + std::to_string(Info->networkSrc->expectedBlockNum) + " enviado";
	Info->userInterface->putNext(auxText);

	Info->timeoutSrc->startTimer();

	genericState *ret = (genericState*) new ST_ReceiveAck();
	return ret;

}

genericState * ST_ReceiveFirstAck::on_Error(genericEvent * ev, usefulInfo * Info)
{	
	//Escribir mensaje de error.

	genericState *ret = (genericState *) new ST_Idle();
	ret->setLastEvent(ERRO);
	return ret;
}

genericState * ST_ReceiveFirstAck::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Timeout, reenviado data package");
	Info->networkInterface->sendPackage(Info->nextPkg);
	return nullptr;
}

//ST_ReceiveAck

genericState * ST_ReceiveAck::on_CloseServer(genericEvent *ev, usefulInfo * Info)
{
	delete Info->nextPkg;
	Info->fileInterface->closeFile();
	setLastEvent(QUIT);
	return nullptr;
}

genericState * ST_ReceiveAck::on_Ack(genericEvent * ev, usefulInfo * Info)
{
	std::string auxText = "Ackowledge #" + std::to_string(Info->networkSrc->blockNumber) + " recibido.";
	Info->userInterface->putNext(auxText);

	Info->networkSrc->expectedBlockNum++;
	delete Info->nextPkg;
	Info->nextPkg = new Data(Info->fileInterface->readData(), Info->networkSrc->expectedBlockNum);
	Info->networkInterface->sendPackage(Info->nextPkg);

	auxText = "Data block #" + std::to_string(Info->networkSrc->expectedBlockNum) + " enviado";
	Info->userInterface->putNext(auxText);
	
	Info->timeoutSrc->startTimer();

	return nullptr;
}

genericState * ST_ReceiveAck::on_Error(genericEvent * ev, usefulInfo * Info)
{
	//Escribir mensaje de error.

	genericState *ret = (genericState *) new ST_Idle();
	ret->setLastEvent(ERRO);
	return ret;
}

genericState * ST_ReceiveAck::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Timeout, reenviado data package");
	Info->networkInterface->sendPackage(Info->nextPkg);
	Info->timeoutSrc->startTimer();
	return nullptr;
}

genericState * ST_ReceiveAck::on_LastData(genericEvent * ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Last data package enviado");
	Info->timeoutSrc->startTimer();
	genericState *ret = (genericState*) new ST_ReceiveLastAck;
	return ret;

}

//ST_ReceiveLastAck

genericState * ST_ReceiveLastAck::on_CloseServer(genericEvent *ev, usefulInfo * Info)
{
	delete Info->nextPkg;
	Info->fileInterface->closeFile();
	setLastEvent(QUIT);
	return nullptr;
}

genericState * ST_ReceiveLastAck::on_Ack(genericEvent * ev, usefulInfo * Info)
{
	std::string auxText = "Ackowledge #" + std::to_string(Info->networkSrc->blockNumber) + " recibido.";
	Info->userInterface->putNext(auxText);
	Info->userInterface->putNext("Transmision completada");

	Info->fileInterface->closeFile();	//Como se finalizo el envio, se cierra el archivo.
	delete Info->nextPkg;	
	Info->timeoutSrc->stopTimer();
	genericState *ret = (genericState*) new ST_Idle;
	return ret;
}

genericState * ST_ReceiveLastAck::on_Error(genericEvent * ev, usefulInfo * Info)
{
	//Escribir mensaje de error.

	genericState *ret = (genericState *) new ST_Idle();
	ret->setLastEvent(ERRO);
	return ret;
}

genericState * ST_ReceiveLastAck::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Timeout, reenviado data package");
	Info->networkInterface->sendPackage(Info->nextPkg);
	return nullptr;
}

//ST_ReceiveFirstData

genericState * ST_ReceiveFirstData::on_CloseServer(genericEvent *ev, usefulInfo * Info)
{
	delete Info->nextPkg;
	Info->fileInterface->closeFile();
	setLastEvent(QUIT);
	return nullptr;
}

genericState * ST_ReceiveFirstData::on_LastData(genericEvent *ev, usefulInfo * Info)
{
	std::string auxText = "Data block #" + std::to_string(Info->networkSrc->blockNumber) + " recibido.";
	Info->userInterface->putNext(auxText);
	Info->userInterface->putNext("Last data package recibido");

	Info->fileInterface->openFile(Info->networkSrc->fileRequested, WRITE);	//abro el archivo.
	Info->fileInterface->saveData(Info->networkSrc->data);	//Escribo el bloque de data en el archivo

	delete Info->nextPkg;
	
	Info->nextPkg = new Acknowledge(Info->networkSrc->expectedBlockNum);	//Construyo el Ack a enviar.
	Info->networkInterface->sendPackage(Info->nextPkg);	//Envio el Ack
	Info->networkSrc->expectedBlockNum++;
	Info->fileInterface->closeFile();
	
	delete Info->nextPkg;

	genericState * ret = (genericState *) new ST_Idle();
	return ret;
}

genericState * ST_ReceiveFirstData::on_Data(genericEvent * ev, usefulInfo * Info)
{
	std::string auxText = "Data block #" + std::to_string(Info->networkSrc->blockNumber) + " recibido.";
	Info->userInterface->putNext(auxText);

	Info->fileInterface->openFile(Info->networkSrc->fileRequested, WRITE);	//abro el archivo.
	Info->fileInterface->saveData(Info->networkSrc->data);	//Escribo el bloque de data en el archivo
	
	delete Info->nextPkg;
	Info->nextPkg = new Acknowledge(Info->networkSrc->expectedBlockNum);	//Construyo el Ack a enviar.
	Info->networkInterface->sendPackage(Info->nextPkg);	//Envio el Ack
	Info->networkSrc->expectedBlockNum++;

	Info->timeoutSrc->startTimer();	//Reseteo timer.

	genericState * ret = (genericState *) new ST_ReceiveData();
	return ret;
}

genericState * ST_ReceiveFirstData::on_Error(genericEvent * ev, usefulInfo * Info)
{
	//Escribir mensaje de error.

	genericState *ret = (genericState *) new ST_Idle();
	ret->setLastEvent(ERRO);
	return ret;
}

genericState * ST_ReceiveFirstData::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Timeout, reenviando acknowledge");
	Info->networkInterface->sendPackage(Info->nextPkg);
	Info->timeoutSrc->startTimer();
	return nullptr;
}

//ST_ReceiveData

genericState * ST_ReceiveData::on_CloseServer(genericEvent *ev, usefulInfo * Info)
{
	delete Info->nextPkg;
	Info->fileInterface->closeFile();
	setLastEvent(QUIT);
	return nullptr;
}

genericState * ST_ReceiveData::on_Data(genericEvent * ev, usefulInfo * Info)
{
	std::string auxText = "Data block #" + std::to_string(Info->networkSrc->blockNumber) + " recibido.";
	Info->userInterface->putNext(auxText);

	Info->fileInterface->openFile(Info->networkSrc->fileRequested, WRITE);	//abro el archivo.
	Info->fileInterface->saveData(Info->networkSrc->data);	//Escribo el bloque de data en el archivo


	delete Info->nextPkg;
	Info->nextPkg = new Acknowledge(Info->networkSrc->expectedBlockNum);	//Construyo el Ack a enviar.
	Info->networkInterface->sendPackage(Info->nextPkg);	//Envio el Ack
	Info->networkSrc->expectedBlockNum++;	//Incremento el blockNumber
	Info->timeoutSrc->startTimer();	//Reseteo timer.

	return nullptr;
}

genericState * ST_ReceiveData::on_Error(genericEvent * ev, usefulInfo * Info)
{
	//Escribir mensaje de error.

	genericState *ret = (genericState *) new ST_Idle();
	ret->setLastEvent(ERRO);
	return ret;
}

genericState * ST_ReceiveData::on_Timeout(genericEvent * ev, usefulInfo * Info)
{
	Info->userInterface->putNext("Timeout, reenviando acknowledge");
	Info->networkInterface->sendPackage(Info->nextPkg);
	Info->timeoutSrc->startTimer();
	return nullptr;
}

genericState * ST_ReceiveData::on_LastData(genericEvent * ev, usefulInfo * Info)
{
	std::string auxText = "Data block #" + std::to_string(Info->networkSrc->blockNumber) + " recibido.";
	Info->userInterface->putNext(auxText);
	Info->userInterface->putNext("Last data package recibido");

	Info->fileInterface->saveData(Info->networkSrc->data);	//Escribo el bloque de data en el archivo
	
	//Info->networkSrc->expectedBlockNum++;
	delete Info->nextPkg;	//Elimino el paquete anterior
	Info->nextPkg = new Acknowledge(Info->networkSrc->expectedBlockNum);	//Construyo el Ack a enviar
	Info->networkInterface->sendPackage(Info->nextPkg);	//Envio el ultimo paquete de data
	Info->fileInterface->closeFile();	//Cierro el archivo
	Info->userInterface->putNext("Transmision completada");

	genericState * ret = (genericState *) new ST_Idle;
	return ret;
}