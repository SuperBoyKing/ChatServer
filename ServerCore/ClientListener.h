#pragma once

#include "IOCPHandler.h"
#include "IOCPOperation.h"

class ChatApplication;

class ClientListener : public IIOCPBinder
{
public:
	ClientListener() = delete;
	ClientListener(const ClientListener& obj) = delete;
	ClientListener(shared_ptr<ChatApplication> chatApp);
	virtual ~ClientListener();

	// Override Function
	inline SOCKET		GetSock() const	override		{ return m_listenSocket; }
	void				ProcessOperation(IOCPOperation* acceptOperation, unsigned int numberOfBytes = 0) override;

	bool		SetUpListener();

	void		ProcessAccept(AcceptOperation* acceptOperation);
	void		RegisterAccept(AcceptOperation* acceptOperation);


private:
	SOCKET						m_listenSocket;
	weak_ptr<ChatApplication>	m_chatApp;
	vector<AcceptOperation*>	m_vAcceptOperations;
};

