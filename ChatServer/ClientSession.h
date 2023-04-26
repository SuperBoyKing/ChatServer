#pragma once
#include "IOCPOperation.h"

class ClientSession : public IIOCPBinder
{
public:
	ClientSession();
	ClientSession(SOCKET clientSocket);
	virtual ~ClientSession();

	// Override Function
	SOCKET		GetSock() const override	{ return m_socket; }
	void		ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) override;

public:
	char*		GetRecvBuffer()				{ return m_recvBuffer; }
	int			GetRecvBufferSize() const	{ return m_recvSize; }
	void		Disconnect();

	inline void	ResetRecvBuffer()			{ memset(m_recvBuffer, 0, MAX_RECV_BUFFER); }

	// Operation 처리
	void		ProcessRecv(unsigned int numberOfBytes);

	// Operation 등록
	void		RegisterRecv();


private:
	SOCKET			m_socket;
	char			m_recvBuffer[MAX_RECV_BUFFER];
	int				m_recvSize;
	IOCPOperation	m_operation;
};

