#include "pch.h"
#include "ClientSessionManager.h"

std::unique_ptr<ClientSessionManager> GClientSessionManager = make_unique<ClientSessionManager>();

ClientSessionManager::ClientSessionManager()
{
}

ClientSessionManager::~ClientSessionManager()
{
	m_setSessions.clear();
}

void ClientSessionManager::Add(shared_ptr<ClientSession> session)
{
	lock_guard<mutex> lock(m_mutex);
	m_setSessions.insert(session);
}

void ClientSessionManager::Remove(shared_ptr<ClientSession> session)
{
	lock_guard<mutex> lock(m_mutex);
	m_setSessions.erase(session);
}