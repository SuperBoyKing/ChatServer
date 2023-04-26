#pragma once
#include "ClientSession.h"

class ClientSessionManager
{
public:
	ClientSessionManager();
	~ClientSessionManager();

	void Add(shared_ptr<ClientSession> session);
	void Remove(shared_ptr<ClientSession> session);
	void Broadcast();

private:
	set<shared_ptr<ClientSession>> m_setSessions;
	mutex m_mutex;
};

extern std::unique_ptr<ClientSessionManager> GClientSessionManager;

