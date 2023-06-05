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
		m_session = m_sessionFactory();
		m_session->SetApp(shared_from_this());

		if (m_session == nullptr)
		{
			PRINT_ERROR("Create Session Error");
			return false;
		}

		if (GetIOCPHandler()->BindIOCompletionPort(m_session) == false)
		{
			PRINT_ERROR("IOCP Bind Error");
			return false;
		}

		if (m_session->Connect() == false)
			return false;
	}

	return true;
}

void ChatClient::SendConnect()
{
	CS_ROOM_LIST_REQUEST packet;
	SendPacket<CS_ROOM_LIST_REQUEST>(packet);
}

void ChatClient::SendRegister(const char* id, const int idSize, const char* pwd, const int pwdSize)
{
	CS_RESGISTER_REQUEST packet;
	::memcpy(packet.userID, id, sizeof(char) * idSize);
	::memcpy(packet.userPW, pwd, sizeof(char) * pwdSize);
	packet.size = sizeof(CS_RESGISTER_REQUEST);

	SendPacket<CS_RESGISTER_REQUEST>(packet);
}

void ChatClient::SendLogin(const char* id, const int idSize, const char* pwd, const int pwdSize)
{
	CS_LOGIN_REQUEST packet;
	::memcpy(packet.userID, id, sizeof(char) * idSize);
	::memcpy(packet.userPW, pwd, sizeof(char) * pwdSize);
	packet.size = sizeof(CS_LOGIN_REQUEST);

	SendPacket<CS_LOGIN_REQUEST>(packet);
}

void ChatClient::SendLogout(const char* id, const int idSize)
{
	CS_LOGOUT_REQUEST packet;
	memcpy(packet.userID, id, idSize);
	packet.size = sizeof(CS_LOGOUT_REQUEST);

	SendPacket<CS_LOGOUT_REQUEST>(packet);
}

void ChatClient::SendChat(const char* str, const int size)
{
	CS_CHAT_REQUEST packet;
	::memcpy(packet.message, str, size);
	packet.size = size + PACKET_HEADER_SIZE;

	SendPacket<CS_CHAT_REQUEST>(packet);
}

void ChatClient::SendRoomOpen(char* title, int titleSize, int userCount)
{
	CS_ROOM_OPEN_REQUEST packet;
	::memcpy(&packet.roomTitle, title, titleSize);
	packet.userCount = userCount;

	SendPacket<CS_ROOM_OPEN_REQUEST>(packet);
}

void ChatClient::SendRoomEnter(int number)
{
	CS_ROOM_ENTER_REQUEST packet;
	packet.roomNumber = number;

	SendPacket<CS_ROOM_ENTER_REQUEST>(packet);
}

void ChatClient::SendRoomLeave(int number)
{
	CS_ROOM_LEAVE_REQUEST packet;
	packet.roomNumber = number;

	SendPacket<CS_ROOM_LEAVE_REQUEST>(packet);
}

void ChatClient::Disconnect(int numberOfThreads)
{
	HANDLE sock = (HANDLE)m_session->GetSock();
	m_session->Disconnect();
	
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
