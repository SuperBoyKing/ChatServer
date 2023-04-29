#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

class IOCPOperation;

ClientSession::ClientSession()
	: m_socket(SocketAssistant::CreateSocket())
	, m_recvBuffer{ 0 }
	, m_recvSize(0)
	, m_sendBuffer{ 0 }
	, m_sendSize(0)
{
	if (m_socket == INVALID_SOCKET) 
		PRINT_WSA_ERROR("Create Socket Error");
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
	GClientSessionManager->Remove(static_pointer_cast<ClientSession>(shared_from_this()));
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

void ClientSession::Connect()
{
	if (SocketAssistant::SetReuseAddress(m_socket) == false)
	{
		PRINT_WSA_ERROR("ReuseAddress Set Error");
		return;
	}

	if (SocketAssistant::SetBindAnyAddress(m_socket, 0) == false)
	{
		PRINT_WSA_ERROR("Any Address Bind Error");
		return;
	}

	RegisterConnect();
}

void ClientSession::Disconnect()
{
	// TEMP
	wcout << "Disconnect : " << m_socket << endl;

	RegisterDisconnect();
}

void ClientSession::send(char* sendBuffer, const unsigned int size)
{
	if (sendBuffer == nullptr)
	{
		cout << "sendBuffer empty" << endl;
		return;
	}

	{
		lock_guard<mutex> lock(m_mutex);
		memcpy(m_sendBuffer, sendBuffer, sizeof(char) * size);
		m_sendSize = size;
	}

	RegisterSend();
}

void ClientSession::ProcessSend(unsigned int numberOfBytes)
{
	//RegisterSend();
}

void ClientSession::ProcessRecv(unsigned int numberOfBytes)
{
	if (numberOfBytes == 0)
	{
		Disconnect();
		return;
	}

	cout << GetRecvBuffer() << ", Sock Number : " << m_socket << endl;
	RegisterRecv();
}

void ClientSession::ProcessConnect()
{
}

void ClientSession::ProcessDisconnect()
{
	m_disconnectOperation.ReleaseOwner();
	GClientSessionManager->Remove(m_socket);
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
	wsaBuf.len = m_sendSize;

	if (SOCKET_ERROR == ::WSASend(m_socket, &wsaBuf, 1, &bytesTransfered, flags, &m_sendOperation, NULL))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Handle Error");
		}
	}

	memset(m_sendBuffer, 0, MAX_BUFFER_SIZE);
	m_sendSize = MAX_BUFFER_SIZE;
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
			PRINT_WSA_ERROR("Handle Error");
		}
	}
}

void ClientSession::RegisterConnect()
{
	// 서버 주소 및 포트 정의
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0)
	{
		cout << "Invalid Server address" << endl;
		return;
	}

	m_connectOperation.Init();
	m_connectOperation.SetType(OperationType::CONNECT);
	m_connectOperation.SetOwner(shared_from_this());

	DWORD sendBytes = 0;
	if (false == SocketAssistant::ConnectEx(m_socket, (sockaddr*)(&serverAddr), sizeof(sockaddr), 
		nullptr, 0, &sendBytes, &m_connectOperation))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			m_connectOperation.ReleaseOwner();
			PRINT_WSA_ERROR("Connection Error");
			return;
		}
	}
	cout << "Connect Success!" << endl;
}

void ClientSession::RegisterDisconnect()
{
	m_disconnectOperation.Init();
	m_disconnectOperation.SetType(OperationType::DISCONNECT);
	m_disconnectOperation.SetOwner(shared_from_this());

	if (false == SocketAssistant::DisConnectEx(m_socket, &m_disconnectOperation, NULL, NULL))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Disconnection Error");
		}
	}	
}
