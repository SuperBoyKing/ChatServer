#include "pch.h"
#include "ChatApplication.h"
#include "ClientListener.h"

ChatApplication::ChatApplication(ApplicationType appType, shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount)
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
	: ChatApplication(ApplicationType::SERVER, serverAddress, iocpHandler, session, maxSessionCount)
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
	: ChatApplication(ApplicationType::CLIENT, serverAddress, iocpHandler, session, maxSessionCount)
{

}

ChatClient::~ChatClient()
{
}

void ChatClient::Connect(const WCHAR* ip, const __int16 port)
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

void ChatClient::SendLogin(const char* id, const int idSize, const char* pwd, const int pwdSize)
{
	CS_LOGIN_REQUEST packet;
	::memcpy(packet.userID, id, sizeof(char) * idSize);
	::memcpy(packet.userPW, pwd, sizeof(char) * pwdSize);
	packet.size += idSize + pwdSize;

	SendPacket<CS_LOGIN_REQUEST>(packet);
}

void ChatClient::SendChat(const char* str, const int size)
{
	CS_CHAT_REQUEST packet;
	::memcpy(packet.message, str, size);
	packet.size += size;

	SendPacket<CS_CHAT_REQUEST>(packet);
}

void ChatClient::SendRoomOpen(int number, char* title, int titleSize, int userCount)
{
	CS_ROOM_OPEN_REQUEST packet;
	packet.roomNumber = number;
	::memcpy(packet.roomTitle, title, titleSize);
	packet.userCount = userCount;

	SendPacket<CS_ROOM_OPEN_REQUEST>(packet);
}

void ChatClient::SendRoomEnter(int number)
{
	CS_ROOM_ENTER_REQUEST packet;
	packet.roomNumber = number;

	SendPacket<CS_ROOM_ENTER_REQUEST>(packet);
}
