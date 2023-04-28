#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

ClientSession::ClientSession()
	: m_socket(SocketAssistant::CreateSocket())
	, m_recvBuffer{ 0 }
	, m_recvSize(0)
	, m_sendBuffer{ 0 }
	, m_sendSize(0)
{
	if (m_socket == INVALID_SOCKET) 
		PRINT_WSA_ERROR("Create Socket Error", WSAGetLastError());
}

ClientSession::ClientSession(SOCKET clientSocket)
	: m_socket(clientSocket)
	, m_recvBuffer{ 0 }
	, m_recvSize(0)
	, m_sendBuffer{ 0 }
	, m_sendSize(0)
{
}

ClientSession::~ClientSession()
{
	SocketAssistant::SocketClose(m_socket);
}

void ClientSession::ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes)
{
	switch (iocpOperation->GetType())
	{
	case OperationType::SEND:
		ProcessSend(numberOfBytes);
		break;
	case OperationType::RECV:
		ProcessRecv(numberOfBytes);
		break;
	case OperationType::CONNECT:
		ProcessConnect();
		break;
	case OperationType::DISCONNECT:
		ProcessDisconnect();
		break;
	default:
		break;
	}
}

bool ClientSession::Connect()
{
	if (SocketAssistant::SetReuseAddress(m_socket) == false)
	{
		PRINT_WSA_ERROR("ReuseAddress Set Error", WSAGetLastError());
		return false;
	}

	if (SocketAssistant::SetBindAnyAddress(m_socket, 0) == false)
	{
		PRINT_WSA_ERROR("Any Address Bind Error", WSAGetLastError());
		return false;
	}

	RegisterConnect();
	return true;
}

void ClientSession::Disconnect()
{
	GClientSessionManager->Remove(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::ProcessSend(unsigned int numberOfBytes)
{
	//RegisterSend();
}

void ClientSession::ProcessRecv(unsigned int numberOfBytes)
{
	cout << GetRecvBuffer() << ", Sock Number : " << m_socket << endl;
	RegisterRecv();
}

void ClientSession::ProcessConnect()
{
}

void ClientSession::ProcessDisconnect()
{
}

void ClientSession::RegisterSend()
{
	m_sendOperation.ReleaseOwner();
	m_sendOperation.Init();
	m_sendOperation.SetOwner(shared_from_this());
	m_sendOperation.SetType(OperationType::SEND);

	DWORD bytesTransfered = 0;
	DWORD flags = 0;

	WSABUF wsaBuf;
	wsaBuf.buf = m_sendBuffer;
	wsaBuf.len = MAX_BUFFER_SIZE;

	if (SOCKET_ERROR == ::WSASend(m_socket, &wsaBuf, 1, &bytesTransfered, flags, &m_sendOperation, NULL))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Handle Error", errorCode);
		}
	}

	memset(m_sendBuffer, 0, MAX_BUFFER_SIZE);
}

void ClientSession::RegisterRecv()
{
	memset(m_recvBuffer, 0, MAX_BUFFER_SIZE);

	m_recvOperation.ReleaseOwner();
	m_recvOperation.Init();
	m_recvOperation.SetOwner(shared_from_this());
	m_recvOperation.SetType(OperationType::RECV);

	DWORD bytesReceived = 0;
	DWORD flags = 0;

	WSABUF wsaBuf;
	wsaBuf.buf = m_recvBuffer;
	wsaBuf.len = MAX_BUFFER_SIZE;

	if (SOCKET_ERROR == ::WSARecv(m_socket, &wsaBuf, 1, &bytesReceived, &flags, &m_recvOperation, NULL))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Handle Error", errorCode);
		}
	}
}

void ClientSession::RegisterConnect()
{
	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(SERVER_PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &sockaddr.sin_addr) <= 0)
	{
		cout << "Invalid address" << endl;
		return;
	}

	while (true)
	{
		int error = connect(m_socket, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			continue;
		}
		else if (error == SOCKET_ERROR)
		{
			PRINT_WSA_ERROR("Connect Error", WSAGetLastError());
			return;
		}
		else
		{
			cout << "Connect Success!" << endl;
			break;
		}
	}
}

void ClientSession::RegisterDisconnect()
{

}
