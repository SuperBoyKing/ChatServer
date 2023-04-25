#pragma once
#include "ClientSession.h"

class ClientSessionManager
{
public:
	ClientSessionManager();
	~ClientSessionManager();

	void Add(ClientSession& session);
	void Remove(ClientSession& session);
	void Broadcast();

private:
	set<ClientSession*> m_setSessions;
};

extern std::unique_ptr<ClientSessionManager> GClientSessionManager;

