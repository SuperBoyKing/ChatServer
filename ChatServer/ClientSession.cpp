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

void ClientSession::ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes)
{
	switch (iocpOperation->GetType())
	{
	case IoType::ACCEPT:
		break;
	case IoType::CONNECT:
		break;
	case IoType::DISCONNECT:
		break;
	case IoType::RECV:
		break;
	case IoType::SEND:
		break;
	default:
		break;
	}
}

void ClientSession::Disconnect()
{
	GClientSessionManager->Remove(*this);
}
