#ifndef STATES_H
#define STATES_H

#include "genericState.h"

class ST_Idle : public genericState
{
public:

	//Chequear estos dos:
	genericState* on_WRQ(genericEvent *ev, usefulInfo * Info);
	genericState* on_RRQ(genericEvent *ev, usefulInfo * Info);
	genericState* on_CloseServer(genericEvent* ev, usefulInfo * Info);
	genericState* on_ClearTerminal(genericEvent *ev, usefulInfo * Info);
	genericState* on_Error(genericEvent *ev, usefulInfo * Info);

};

class ST_ReceiveFirstAck :public genericState
{
public:

	genericState* on_Ack(genericEvent* ev, usefulInfo * Info);
	genericState* on_Error(genericEvent* ev, usefulInfo * Info);
	genericState* on_Timeout(genericEvent* ev, usefulInfo * Info);
	genericState* on_LastData(genericEvent* ev, usefulInfo * Info);

	std::string getSelectedFile() { return fileToTransfer; }
	void setFileToTransfer(std::string selectedFile) { this->fileToTransfer = selectedFile; }
private:
	std::string fileToTransfer;
};

class ST_ReceiveAck : public genericState
{
public:

	genericState* on_Ack(genericEvent* ev, usefulInfo * Info);
	genericState* on_Error(genericEvent* ev, usefulInfo * Info);
	genericState* on_Timeout(genericEvent* ev, usefulInfo * Info);
	genericState* on_LastData(genericEvent* ev, usefulInfo * Info);

private:
};

class ST_ReceiveLastAck : public genericState
{
public:

	genericState* on_Ack(genericEvent* ev, usefulInfo * Info);
	genericState* on_Error(genericEvent* ev, usefulInfo * Info);
	genericState* on_Timeout(genericEvent* ev, usefulInfo * Info);

private:

};

class ST_ReceiveFirstData : public genericState
{
public:

	genericState* on_LastData(genericEvent* ev, usefulInfo * Info);
	genericState* on_Data(genericEvent* ev, usefulInfo * Info);
	genericState* on_Error(genericEvent* ev, usefulInfo * Info);
	genericState* on_Timeout(genericEvent* ev, usefulInfo * Info);
private:
};

class ST_ReceiveData : public genericState
{
public:

	genericState* on_Data(genericEvent* ev, usefulInfo * Info);
	genericState* on_Error(genericEvent* ev, usefulInfo * Info);
	genericState* on_Timeout(genericEvent* ev, usefulInfo * Info);
	genericState* on_LastData(genericEvent* ev, usefulInfo * Info);

private:
};

#endif // !STATES_H