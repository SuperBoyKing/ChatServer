#pragma once
#include "IOCPHandler.h"
#include "IOCPOperation.h"

class ClientListener : public IIOCPBinder
{
public:
	ClientListener();
	ClientListener(const WCHAR* ip, const unsigned __int16 port);
	virtual ~ClientListener();

	void		Init();

	// Override Function
	inline SOCKET		GetSock() const	override		{ return m_ListenSocket; }
	void				ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) override;

	void		ProcessAccept();


private:
	SOCKET			m_ListenSocket;
	ServerAddress*	m_serverAddress;
};

