#include "pch.h"
#include "ChatSession.h"
#include "ClientSessionManager.h"

class IOCPOperation;

ChatSession::ChatSession()
	: m_socket(SocketAssistant::CreateSocket())
	, m_recvBuffer(MAX_RECV_BUFFER_SIZE)
	, m_isConnected(false)
	, m_isRegisteredSend(false)
	, m_chatApp(shared_ptr<ChatApplication>(nullptr))
{
	if (m_socket == INVALID_SOCKET)
		PRINT_WSA_ERROR("Create Socket Error");
}

ChatSession::~ChatSession()
{
	SocketAssistant::SocketClose(m_socket);
}

void ChatSession::ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes)
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

bool ChatSession::Connect()
{
	if (IsConnected())
	{
		PRINT_ERROR("Client already connected");
		return false;
	}

	if (CHAT_APPLICATION_TYPE != ApplicationType::CLIENT)
	{
		PRINT_ERROR("Invalid Application Type");
		return false;
	}

	if (SocketAssistant::SetReuseAddress(m_socket) == false)
	{
		PRINT_WSA_ERROR("ReuseAddress Set Error");
		return false;
	}

	if (SocketAssistant::SetBindAnyAddress(m_socket, 0) == false)
	{
		PRINT_WSA_ERROR("Any Address Bind Error");
		return false;
	}

	return RegisterConnect();
}

void ChatSession::Disconnect()
{
	if (m_isConnected.exchange(false) == false)
		return;

	wcout << "Disconnect : " << m_socket << endl;

	RegisterDisconnect();
}

void ChatSession::Send(shared_ptr<SendBuffer> sendbuffer)
{
	if (IsConnected() == false)
		return;

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

void ChatSession::ProcessSend(unsigned int numberOfBytes)
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

void ChatSession::ProcessRecv(unsigned int numberOfBytes)
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

void ChatSession::ProcessConnect()
{
	m_connectOperation.ReleaseOwner();

	m_isConnected.store(true);
	
	OnConnect();	// 연결된 클라이언트 추가

	RegisterRecv();
}

void ChatSession::ProcessDisconnect()
{
	m_disconnectOperation.ReleaseOwner();

	OnDisconnect(); // 연결 해제된 클라이언트 삭제

	m_isConnected.store(false);
}

void ChatSession::RegisterSend()
{
	if (IsConnected() == false)
		return;

	DWORD bytesTransfered = 0;
	DWORD flags = 0;

	// SendQueue에 있는 버퍼공유포인터를 Operation의 vector로 복사
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

void ChatSession::RegisterRecv()
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

bool ChatSession::RegisterConnect()
{
	m_connectOperation.Init();
	m_connectOperation.SetOwner(shared_from_this());

	DWORD sendBytes = 0;
	sockaddr_in serverAddr = CHAT_ADDRESS->GetSockAddrIn();
	if (false == SocketAssistant::ConnectEx(m_socket, (sockaddr*)(&serverAddr), sizeof(sockaddr), 
		nullptr, 0, &sendBytes, &m_connectOperation))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			m_connectOperation.ReleaseOwner();
			PRINT_WSA_ERROR("Connection Error");
			return false;
		}
	}

	cout << "Connect Success!" << endl;
	return true;
}

void ChatSession::RegisterDisconnect()
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
