#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

ClientSession::ClientSession() 
	: m_socket(SocketAssistant::CreateSocket())
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
	SocketAssistant::SocketClose(m_socket);
}

void ClientSession::ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes)
{
	m_operation.ReleaseOwner();

	switch (iocpOperation->GetType())
	{
	case OperationType::SEND:
		break;
	case OperationType::RECV:
		ProcessRecv(numberOfBytes);
		break;
	case OperationType::CONNECT:
		break;
	case OperationType::DISCONNECT:
		break;
	default:
		break;
	}
}

void ClientSession::Disconnect()
{
	GClientSessionManager->Remove(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::ProcessRecv(unsigned int numberOfBytes)
{
	cout << GetRecvBuffer() << ", Length : " << numberOfBytes << endl;
	RegisterRecv();
}

void ClientSession::RegisterRecv()
{
	ResetRecvBuffer();

	m_operation.Init();
	m_operation.SetOwner(shared_from_this());
	m_operation.SetType(OperationType::RECV);

	DWORD recvLen = 0;
	DWORD key = 0;

	WSABUF wsaBuf;
	wsaBuf.buf = GetRecvBuffer();
	wsaBuf.len = MAX_RECV_BUFFER;

	if (::WSARecv(GetSock(), &wsaBuf, 1, &recvLen, &key, &m_operation, NULL))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Handle Error", errorCode);
		}
	}
}
