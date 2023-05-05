#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

class IOCPOperation;

ClientSession::ClientSession()
	: m_socket(SocketAssistant::CreateSocket())
	, m_recvBuffer(MAX_BUFFER_SIZE)
	, m_sendBuffer{ 0 }
	, m_sendSize(0)
	, m_isConnected(false)
{
	if (m_socket == INVALID_SOCKET)
		PRINT_WSA_ERROR("Create Socket Error");
}

ClientSession::ClientSession(SOCKET clientSocket)
	: m_socket(clientSocket)
	, m_recvBuffer(MAX_BUFFER_SIZE)
	, m_sendBuffer{ 0 }
	, m_sendSize(0)
	, m_isConnected(false)
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
		ProcessSend(static_cast<SendOperation*>(iocpOperation), numberOfBytes);
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
	if (m_isConnected.exchange(false) == false)
		return;

	wcout << "Disconnect : " << m_socket << endl;

	RegisterDisconnect();
}

void ClientSession::Send(const char* sendBuffer, const unsigned int len)
{
	SendOperation* sendOperation = new SendOperation();
	sendOperation->buffer.resize(len);
	sendOperation->SetOwner(shared_from_this());
	::memcpy(sendOperation->buffer.data(), sendBuffer, len);

	lock_guard<mutex> lock(m_mutex);
	RegisterSend(sendOperation);
}

void ClientSession::ProcessSend(SendOperation* sendOperation, unsigned int numberOfBytes)
{
	sendOperation->ReleaseOwner();
	delete sendOperation;

	if (numberOfBytes == 0)
	{
		Disconnect();
		return;
	}
}

void ClientSession::ProcessRecv(unsigned int numberOfBytes)
{
	m_recvOperation.ReleaseOwner();

	if (numberOfBytes == 0)
	{
		Disconnect();
		return;
	}

	if (m_recvBuffer.OnWrite(numberOfBytes) == false)
	{
		Disconnect();
		return;
	}

	int dataSize = m_recvBuffer.GetDataSize();
    OnRecv(m_recvBuffer.GetReadPos(), dataSize);

	if (dataSize <= 0 || m_recvBuffer.OnRead(numberOfBytes) == false)
	{
		Disconnect();
		return;
	}

	m_recvBuffer.Clear();

	RegisterRecv();
}

void ClientSession::ProcessConnect()
{
	m_connectOperation.ReleaseOwner();

	m_isConnected.store(true);
	
	GClientSessionManager->Add(static_pointer_cast<ClientSession>(shared_from_this()));	// 연결된 클라이언트 추가

	RegisterRecv();
}

void ClientSession::ProcessDisconnect()
{
	m_disconnectOperation.ReleaseOwner();

	GClientSessionManager->Remove(m_socket);
}

void ClientSession::RegisterSend(SendOperation* sendOperation)
{
	if (IsConnected() == false)
		return;

	DWORD bytesTransfered = 0;
	DWORD flags = 0;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendOperation->buffer.data();
	wsaBuf.len = (ULONG)sendOperation->buffer.size();

	if (SOCKET_ERROR == ::WSASend(m_socket, &wsaBuf, 1, &bytesTransfered, flags, sendOperation, NULL))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Handle Error");
			sendOperation->ReleaseOwner();
			delete sendOperation;
		}
	}
}

void ClientSession::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	m_recvOperation.Init();
	m_recvOperation.SetOwner(shared_from_this());

	DWORD bytesReceived = 0;
	DWORD flags = 0;

	WSABUF wsaBuf;
	wsaBuf.buf = m_recvBuffer.GetWritePos();
	wsaBuf.len = m_recvBuffer.GetBufferSize();

	if (SOCKET_ERROR == ::WSARecv(m_socket, &wsaBuf, 1, &bytesReceived, &flags, &m_recvOperation, NULL))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Handle Error");
			m_recvOperation.ReleaseOwner();
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
	m_disconnectOperation.SetOwner(shared_from_this());

	if (false == SocketAssistant::DisConnectEx(m_socket, &m_disconnectOperation, NULL, NULL))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			m_disconnectOperation.ReleaseOwner();
			PRINT_WSA_ERROR("Disconnection Error");
			return;
		}
	}	
}
