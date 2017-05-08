#ifndef STATES_H
#define STATES_H

#include "genericState.h"

class ST_Idle : public genericState
{
public:

	//Chequear estos dos:
	//genericState* on_SendError(genericEvent *ev);
	//genericState* on_ReceiveError(genericEvent *ev);

	genericState* on_CloseServer(genericEvent* ev);
	genericState* on_ClearTerminal(genericEvent *ev);


};

class ST_ReceiveFirstAck :public genericState
{
public:

	genericState* on_Ack(genericEvent* ev);
	genericState* on_Error(genericEvent* ev);
	genericState* on_Timeout(genericEvent* ev);
	genericState* on_ConnectionFailed(genericEvent* ev);

	std::string getSelectedFile() { return fileToTransfer; }
	void setFileToTransfer(std::string selectedFile) { this->fileToTransfer = selectedFile; }
private:
	std::string fileToTransfer;
};

class ST_ReceiveAck : public genericState
{
public:

	genericState* on_Ack(genericEvent* ev);
	genericState* on_Error(genericEvent* ev);
	genericState* on_Timeout(genericEvent* ev);
	genericState* on_ConnectionFailed(genericEvent* ev);
	genericState* on_LastData(genericEvent* ev);

private:
};

class ST_ReceiveLastAck : public genericState
{
public:

	genericState* on_Ack(genericEvent* ev);
	genericState* on_Error(genericEvent* ev);
	genericState* on_Timeout(genericEvent* ev);
	genericState* on_ConnectionFailed(genericEvent* ev);

private:

};

class ST_ReceiveFirstData : public genericState
{
public:

	genericState* on_Data(genericEvent* ev);
	genericState* on_Error(genericEvent* ev);
	genericState* on_Timeout(genericEvent* ev);
	genericState* on_ConnectionFailed(genericEvent* ev);
private:
};

class ST_ReceiveData : public genericState
{
public:

	genericState* on_Data(genericEvent* ev);
	genericState* on_Error(genericEvent* ev);
	genericState* on_Timeout(genericEvent* ev);
	genericState* on_ConnectionFailed(genericEvent* ev);
	genericState* on_LastData(genericEvent* ev);

private:
};

#endif // !STATES_H