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
	m_operation.ReleaseOwner();

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
	cout << GetRecvBuffer() << ", Length : " << numberOfBytes << endl;
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
	cout << "send Msg : ";
	cin.getline(m_sendBuffer, sizeof(m_sendBuffer), '\n');

	int sendBytes = ::send(m_socket, m_sendBuffer, sizeof(m_sendBuffer), 0);
	if (sendBytes != SOCKET_ERROR)
	{
		cout << "send bytes : " << sendBytes << endl;
	}
	else
	{
		PRINT_WSA_ERROR("Send Error", WSAGetLastError());
	}

	memset(m_sendBuffer, 0, MAX_BUFFER_SIZE);
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
	wsaBuf.len = MAX_BUFFER_SIZE;

	if (::WSARecv(GetSock(), &wsaBuf, 1, &recvLen, &key, &m_operation, NULL))
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
