#include "pch.h"
#include "ChatApplication.h"
#include "ClientListener.h"

ChatApplication::ChatApplication(ApplicationType appType, shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, int maxSessionCount)
	: m_appType(appType)
	, m_serverAddress(serverAddress)
	, m_iocpHandler(iocpHandler)
	, m_maxSessionCount(maxSessionCount)
{
}

ChatApplication::~ChatApplication()
{
}

ChatServer::ChatServer(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, int maxSessionCount)
	: ChatApplication(ApplicationType::SERVER, serverAddress, iocpHandler, maxSessionCount)
	, m_clientListener(nullptr)
{
}

ChatServer::~ChatServer()
{
}

bool ChatServer::Start()
{
	m_clientListener = make_shared<ClientListener>();
	if (m_clientListener == nullptr)
	{
		PRINT_ERROR("Create Listener Error");
		return false;
	}
	shared_ptr<ChatServer> chatServer = static_pointer_cast<ChatServer>(shared_from_this());
	m_clientListener->SetUpListener(static_pointer_cast<ChatServer>(shared_from_this()), GetMaxSessionCount());
	if (GetIOCPHandler()->BindIOCompletionPort(m_clientListener) == false)
	{
		PRINT_ERROR("IOCP Bind Error");
		return false;
	}

	return true;
}

ChatClient::ChatClient(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, int maxSessionCount)
	: ChatApplication(ApplicationType::CLIENT, serverAddress, iocpHandler, maxSessionCount)
{

}

ChatClient::~ChatClient()
{
}

bool ChatClient::Start()
{
	return true;
}
