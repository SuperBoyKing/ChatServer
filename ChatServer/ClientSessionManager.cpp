#include "pch.h"
#include "ClientSessionManager.h"

ClientSessionManager::ClientSessionManager()
{
}

ClientSessionManager::~ClientSessionManager()
{
	m_setSessions.clear();
}

void ClientSessionManager::Add(ClientSession& session)
{
	recursive_mutex mutex;
	scoped_lock<recursive_mutex> lock(mutex);
	m_setSessions.insert(session);
}

void ClientSessionManager::Remove(ClientSession& session)
{
	mutex m;
	lock_guard<mutex> lock(m);
	m_setSessions.erase(session);
}