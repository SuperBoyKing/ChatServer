#pragma once
#include "IOCPOperation.h"
#include "IOCPServer.h"

class ClientSession : public IOCPBinder
{
public:
	ClientSession();
	ClientSession(SOCKET clientSocket);
	~ClientSession();

	// Override Function
	SOCKET		GetSock() const override	{ return m_socket; }
	void		ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) override;

public:
	char*		GetRecvBuffer()				{ return m_recvBuffer; }
	int			GetRecvBufferSize() const	{ return m_recvSize; }
	void		Disconnect();

private:
	SOCKET		m_socket;
	char		m_recvBuffer[MAX_RECV_BUFFER];
	int			m_recvSize;
};

