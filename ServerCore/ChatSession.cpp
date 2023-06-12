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
	, m_userID{}
	, m_roomNumber(0)
	, m_sessionState(SessionState::NONE)
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
	case OperationType::DB_RESPONSE:
		ProcessDBResponse(static_cast<DBResOperation*>(iocpOperation), numberOfBytes);
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

	if (CHAT_SESSION_TYPE != SessionType::CLIENT)
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
	OnDisconnect();

	if (m_isConnected.exchange(false) == false)
		return;

	wcout << "Disconnect : " << m_socket << endl;

	RegisterDisconnect();
}

bool ChatSession::Send(shared_ptr<SendBuffer> sendbuffer)
{
	const ULONGLONG startTimeOutTick = ::GetTickCount64();
	while (true) // ConnectEx를 통한 비동기 소켓이 Connection 될 때 까지 대기
	{
		if (IsConnected())
			break;

		if (::GetTickCount64() - startTimeOutTick >= CONNECTION_TIME_OUT_TICK)
		{
			CRASH("Connection Time Out");
			return false;
		}

		this_thread::yield();
	}

	bool registSend = false;

	{
		lock_guard<recursive_mutex> lock(m_mutex);
		m_sendQueue.push(sendbuffer);

		if (m_isRegisteredSend.exchange(true) == false)
			registSend = true;
	}
	
	if (registSend == true)
		return RegisterSend();	

	return false;
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
	
	OnConnect();	

	RegisterRecv();
}

void ChatSession::ProcessDisconnect()
{
	m_disconnectOperation.ReleaseOwner();

	m_isConnected.store(false);

	m_sessionState = SessionState::NONE;
	
	::memset(m_userID, 0, sizeof(char) * MAX_USER_ID_LENGTH);

	if (CHAT_SESSION_TYPE == SessionType::CLIENT)
	{
		SC_CONNECT_RESPONSE sendPacket;
		sendPacket.result = false; // 서버로 부터 연결 끊어짐
		OnRecv(reinterpret_cast<char*>(&sendPacket), sizeof(SC_CONNECT_RESPONSE));
	}
}

void ChatSession::ProcessDBResponse(DBResOperation* dbOperation, unsigned int numberOfBytes)
{
	OnRecv(reinterpret_cast<char*>(dbOperation->sendBuffer->GetBuffer()), numberOfBytes);

	dbOperation->ReleaseOwner();

	delete dbOperation;
}

bool ChatSession::RegisterSend()
{
	if (IsConnected() == false)
		return false;

	DWORD bytesTransfered = 0;
	DWORD flags = 0;

	// SendQueue에 있는 버퍼공유포인터를 SendOperation의 vector로 복사
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
		OnSend(m_sendOperation.sendBuffers[i]->GetBuffer());
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
			return false;
		}
	}

	return true;
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

	if (false == SocketAssistant::DisConnectEx(m_socket, &m_disconnectOperation, TF_REUSE_SOCKET, NULL))
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
