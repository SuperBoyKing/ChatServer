#include "pch.h"
#include "ClientSessionManager.h"

std::unique_ptr<ClientSessionManager> GClientSessionManager = make_unique<ClientSessionManager>();

ClientSessionManager::ClientSessionManager()
{
}

ClientSessionManager::~ClientSessionManager()
{
	m_uMapSessions.clear();
}

void ClientSessionManager::Add(shared_ptr<ChatSession> session)
{
	lock_guard<recursive_mutex> lock(m_mutex);
	m_uMapSessions.insert(pair<SOCKET, shared_ptr<ChatSession>>(session->GetSock(), session));
}

void ClientSessionManager::Remove(shared_ptr<ChatSession> session)
{
	lock_guard<recursive_mutex> lock(m_mutex);
	m_uMapSessions.erase(session->GetSock());
}

void ClientSessionManager::Remove(SOCKET key)
{
	lock_guard<recursive_mutex> lock(m_mutex);
	m_uMapSessions.erase(key);
}

shared_ptr<ChatSession> ClientSessionManager::Search(SOCKET key)
{
	auto itr = m_uMapSessions.find(key);
	if (itr == m_uMapSessions.end())
	{
		return shared_ptr<ChatSession>();
	}
	
	if (itr->second.use_count() == 0)	// 참조 카운트가 0일 경우 해쉬에서 삭제
	{
		Remove(key);
		return shared_ptr<ChatSession>();
	}

	return itr->second;
}

void ClientSessionManager::Broadcast(shared_ptr<SendBuffer> sendBuffer, shared_ptr<ChatSession> exceptSession)
{
	for (auto &clients : m_uMapSessions)
	{
		if (exceptSession.get() != clients.second.get())
			clients.second->Send(sendBuffer);
	}
}

bool ClientSessionManager::SendToSession(shared_ptr<SendBuffer> sendBuffer, shared_ptr<ChatSession> session)
{
	lock_guard<recursive_mutex> lock(m_mutex); 
	if (session.get() != nullptr)
	{
		session->Send(sendBuffer);
		return true;
	}

	return false;
}
