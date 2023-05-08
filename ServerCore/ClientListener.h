#pragma once
#include "IOCPHandler.h"
#include "IOCPOperation.h"
#include "ChatApplication.h"

class ClientListener : public IIOCPBinder
{
public:
	ClientListener();
	virtual ~ClientListener();

	void		SetUpListener(shared_ptr<ChatServer> chatServer, const unsigned int maxClientSession = 1000);

	// Override Function
	inline SOCKET		GetSock() const	override		{ return m_listenSocket; }
	void				ProcessOperation(IOCPOperation* acceptOperation, unsigned int numberOfBytes = 0) override;

	void		ProcessAccept(AcceptOperation* acceptOperation);
	void		RegisterAccept(AcceptOperation* acceptOperation);


private:
	SOCKET						m_listenSocket;
	//ServerAddress				m_serverAddress;
	shared_ptr<ChatServer>		m_chatServer;
	vector<AcceptOperation*>	m_vAcceptOperations;
};

