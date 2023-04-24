#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

ClientSession::ClientSession() 
	: m_socket(INVALID_SOCKET)
	, m_recvBuffer{0}
	, m_recvSize(0)
{
}

ClientSession::ClientSession(SOCKET clientSocket)
	: m_socket(clientSocket)
	, m_recvBuffer{0}
	, m_recvSize(0)
{
}

ClientSession::~ClientSession()
{
	closesocket(m_socket);
}

void ClientSession::Disconnect()
{
	GClientSessionManager->Remove(*this);
}
