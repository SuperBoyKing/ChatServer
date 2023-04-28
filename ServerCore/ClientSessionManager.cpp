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

void ClientSessionManager::Add(shared_ptr<ClientSession> session)
{
	lock_guard<mutex> lock(m_mutex);
	m_uMapSessions.insert(pair<SOCKET, shared_ptr<ClientSession>>(session->GetSock(), session));
}

void ClientSessionManager::Remove(shared_ptr<ClientSession> session)
{
	lock_guard<mutex> lock(m_mutex);
	m_uMapSessions.erase(session->GetSock());
}

void ClientSessionManager::Remove(SOCKET key)
{
	lock_guard<mutex> lock(m_mutex);
	m_uMapSessions.erase(key);
}

shared_ptr<ClientSession> ClientSessionManager::Search(SOCKET key)
{
	auto itr = m_uMapSessions.find(key);
	if (itr == m_uMapSessions.end())
	{
		return shared_ptr<ClientSession>();
	}
	
	if (itr->second.use_count() == 0)	// 참조 카운트가 0일 경우 해당 댕글링 포인터를 해쉬에서 삭제
	{
		Remove(key);
		return shared_ptr<ClientSession>();
	}

	return itr->second;
}

void ClientSessionManager::Broadcast()
{

}