#pragma once
#include "ChatSession.h"

class ClientSessionManager
{
public:
	ClientSessionManager();
	~ClientSessionManager();

	void						Add(shared_ptr<ChatSession> session);
	void						Remove(shared_ptr<ChatSession> session);
	void						Remove(SOCKET key);
	shared_ptr<ChatSession>		Search(SOCKET key);
	void						Broadcast(shared_ptr<SendBuffer>& sendBuffer, shared_ptr<ChatSession> exceptSession = nullptr);
	bool						SendToSession(shared_ptr<SendBuffer>& sendBuffer, shared_ptr<ChatSession>& session);

private:
	unordered_map<SOCKET, shared_ptr<ChatSession>> m_uMapSessions;
	recursive_mutex m_mutex;
	HANDLE m_iocpHandle;
};

extern std::unique_ptr<ClientSessionManager> GClientSessionManager;