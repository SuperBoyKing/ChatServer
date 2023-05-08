#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

class IOCPOperation;

ClientSession::ClientSession()
	: m_socket(SocketAssistant::CreateSocket())
	, m_recvBuffer(MAX_RECV_BUFFER_SIZE)
	, m_isConnected(false)
	, m_isRegisteredSend(false)
{
	if (m_socket == INVALID_SOCKET)
		PRINT_WSA_ERROR("Create Socket Error");
}

ClientSession::ClientSession(SOCKET clientSocket)
	: m_socket(clientSocket)
	, m_recvBuffer(MAX_RECV_BUFFER_SIZE)
	, m_isConnected(false)
	, m_isRegisteredSend(false)
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

void ClientSession::Send(shared_ptr<SendBuffer> sendbuffer)
{
	m_sendOperation.SetOwner(shared_from_this());

	bool registSend = false;

	{
		lock_guard<recursive_mutex> lock(m_mutex);
		m_sendQueue.push(sendbuffer);

		if (m_isRegisteredSend.exchange(true) == false)
			registSend = true;
	}
	
	if (registSend == true)
		RegisterSend();	
}

void ClientSession::ProcessSend(unsigned int numberOfBytes)
{
	m_sendOperation.ReleaseOwner();
	m_sendOperation.sendBuffers.clear();

	if (numberOfBytes == 0)
	{
		Disconnect();
		return;
	}

	OnSend(numberOfBytes);

	lock_guard<recursive_mutex> lock(m_mutex);
	if (m_sendQueue.empty())
		m_isRegisteredSend.store(false);
	else
		RegisterSend();
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

void ClientSession::RegisterSend()
{
	if (IsConnected() == false)
		return;

	DWORD bytesTransfered = 0;
	DWORD flags = 0;

	// SendQueue에 있는 버퍼공유포인터를 Operation에 vector로 복사
	{
		lock_guard<recursive_mutex> lock(m_mutex);

		m_sendOperation.Init();
		m_sendOperation.SetOwner(shared_from_this());

		int	writeSize = 0;
		while (m_sendQueue.empty() == false)
		{
			shared_ptr<SendBuffer> sendBuffer = m_sendQueue.front();
			writeSize = sendBuffer->GetWriteSize();
			ASSERT_CRASH(writeSize < MAX_SEND_BUFFER_SIZE);

			m_sendQueue.pop();
			m_sendOperation.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scattered-gathered I/O (vecterd I/O)
	vector<WSABUF> wsaBufs;
	wsaBufs.resize(m_sendOperation.sendBuffers.size());

	for (int i = 0; i < m_sendOperation.sendBuffers.size(); ++i)
	{
		wsaBufs[i].buf = reinterpret_cast<CHAR*>(m_sendOperation.sendBuffers[i]->GetBuffer());
		wsaBufs[i].len = static_cast<ULONG>(m_sendOperation.sendBuffers[i]->GetWriteSize());
	}

	if (SOCKET_ERROR == ::WSASend(m_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), &bytesTransfered, flags, &m_sendOperation, NULL))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Handle Error");
			m_sendOperation.ReleaseOwner();
			m_sendOperation.sendBuffers.clear();
			m_isRegisteredSend.store(false);
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
