#pragma once
#include "IOCPHandler.h"
#include "IOCPOperation.h"

class ClientListener : public IIOCPBinder
{
public:
	ClientListener();
	ClientListener(const WCHAR* ip, const unsigned __int16 port);
	virtual ~ClientListener();

	void		SetUpListener(const unsigned int maxClientSession = 1000);

	// Override Function
	inline SOCKET		GetSock() const	override		{ return m_listenSocket; }
	void				ProcessOperation(IOCPOperation* acceptOperation, unsigned int numberOfBytes = 0) override;

	void		ProcessAccept(AcceptOperation* acceptOperation);
	void		RegisterAccept(AcceptOperation* acceptOperation);


private:
	SOCKET						m_listenSocket;
	ServerAddress				m_serverAddress;
	vector<AcceptOperation*>	m_vAcceptOperations;
};

