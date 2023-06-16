#include "pch.h"
#include "ChatApplication.h"
#include "ClientListener.h"

ChatApplication::ChatApplication(SessionType appType, shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount)
	: m_appType(appType)
	, m_serverAddress(serverAddress)
	, m_iocpHandler(iocpHandler)
	, m_sessionFactory(session)
	, m_maxSessionCount(maxSessionCount)
{
}

ChatApplication::~ChatApplication()
{
}

ChatServer::ChatServer(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount)
	: ChatApplication(SessionType::SERVER, serverAddress, iocpHandler, session, maxSessionCount)
	, m_clientListener(nullptr)
{
}

ChatServer::~ChatServer()
{
}

bool ChatServer::Start()
{
	m_clientListener = make_shared<ClientListener>(shared_from_this());

	if (m_clientListener == nullptr)
	{
		PRINT_ERROR("Create Listener Error");
		return false;
	}
	else
	{
		if (m_clientListener->SetUpListener() == false)
			return false;
	}

	if (GetIOCPHandler()->BindIOCompletionPort(m_clientListener) == false)
	{
		PRINT_ERROR("IOCP Bind Error");
		return false;
	}

	return true;
}

ChatClient::ChatClient(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount)
	: ChatApplication(SessionType::CLIENT, serverAddress, iocpHandler, session, maxSessionCount)
{
	m_vSessions.reserve(maxSessionCount);
}

ChatClient::~ChatClient()
{
}

void ChatClient::SetAddress(const WCHAR* ip, const __int16 port)
{
	if (ip != nullptr)
		m_serverAddress->SetServerAddress(ip, port);
}

bool ChatClient::Start()
{
	for (int i = 0; i < m_maxSessionCount; ++i)
	{
		m_vSessions.push_back(m_sessionFactory());
		m_vSessions[i]->SetApp(shared_from_this());

		if (m_vSessions[i] == nullptr)
		{
			PRINT_ERROR("Create Session Error");
			return false;
		}

		if (GetIOCPHandler()->BindIOCompletionPort(m_vSessions[i]) == false)
		{
			PRINT_ERROR("IOCP Bind Error");
			return false;
		}

		if (m_vSessions[i]->Connect() == false)
			return false;
	}

	return true;
}

bool ChatClient::SendConnect()
{
	CS_CONNECT_REQUEST packet;
	return SendPacket(packet);
}

void ChatClient::SendRoomList()
{
	CS_ROOM_LIST_REQUEST packet;
	SendPacket<CS_ROOM_LIST_REQUEST>(packet);
}

void ChatClient::SendRegister(const char* id, const int idSize, const char* pwd, const int pwdSize, const int sessionID)
{
	CS_RESGISTER_REQUEST packet;
	::memcpy(packet.userID, id, sizeof(char) * idSize);
	::memcpy(packet.userPW, pwd, sizeof(char) * pwdSize);
	packet.size = sizeof(CS_RESGISTER_REQUEST);

	SendPacket<CS_RESGISTER_REQUEST>(packet, sessionID);
}

void ChatClient::SendLogin(const char* id, const int idSize, const char* pwd, const int pwdSize, const int sessionID)
{
	CS_LOGIN_REQUEST packet;
	::memcpy(packet.userID, id, sizeof(char) * idSize);
	::memcpy(packet.userPW, pwd, sizeof(char) * pwdSize);
	packet.size = sizeof(CS_LOGIN_REQUEST);

	SendPacket<CS_LOGIN_REQUEST>(packet, sessionID);
}

void ChatClient::SendLogout(const char* id, const int idSize)
{
	CS_LOGOUT_REQUEST packet;
	memcpy(packet.userID, id, idSize);
	packet.size = sizeof(CS_LOGOUT_REQUEST);

	SendPacket<CS_LOGOUT_REQUEST>(packet);
}

void ChatClient::SendChat(const char* str, const int size, const int sessionID)
{
	CS_CHAT_REQUEST packet;
	::memcpy(packet.message, str, size);
	packet.size = size + PACKET_HEADER_SIZE;

	SendPacket<CS_CHAT_REQUEST>(packet, sessionID);
}

void ChatClient::SendRoomOpen(const char* title, const int titleSize, const int userCount)
{
	CS_ROOM_OPEN_REQUEST packet;
	::memcpy(&packet.roomTitle, title, titleSize);
	packet.userCount = userCount;

	SendPacket<CS_ROOM_OPEN_REQUEST>(packet);
}

void ChatClient::SendRoomEnter(const int number, const int sessionID)
{
	CS_ROOM_ENTER_REQUEST packet;
	packet.roomNumber = number;

	SendPacket<CS_ROOM_ENTER_REQUEST>(packet, sessionID);
}

void ChatClient::SendRoomLeave(const int number, const int sessionID)
{
	CS_ROOM_LEAVE_REQUEST packet;
	packet.roomNumber = number;

	SendPacket<CS_ROOM_LEAVE_REQUEST>(packet, sessionID);
}

void ChatClient::Disconnect(int numberOfThreads, int sessionID)
{
	for (int i = 0; i < m_maxSessionCount; ++i)
	{
		m_vSessions[i]->Disconnect();
	}
	m_vSessions.clear();
	
	//GQCS 반환을 위한 PQCS 처리
	for (int i = 0; i < numberOfThreads; ++i)
	{
		if (false == ::PostQueuedCompletionStatus(m_iocpHandler->GetIocpHandle(), NULL, NULL, NULL))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				PRINT_WSA_ERROR("PostQueuedCompletionStatus Error");
				return;
			}
		}
	}
}
