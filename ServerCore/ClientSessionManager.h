#pragma once
#include "ClientSession.h"

class ClientSessionManager
{
public:
	ClientSessionManager();
	~ClientSessionManager();

	void						Add(shared_ptr<ClientSession> session);
	void						Remove(shared_ptr<ClientSession> session);
	void						Remove(SOCKET key);
	shared_ptr<ClientSession>	Search(SOCKET key);
	void						Broadcast();

private:
	unordered_map<SOCKET, shared_ptr<ClientSession>> m_uMapSessions;
	mutex m_mutex;
};

extern std::unique_ptr<ClientSessionManager> GClientSessionManager;