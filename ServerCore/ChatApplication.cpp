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

bool ChatClient::Start()
{
	int maxSessionCount = GetMaxSessionCount();

	for (int i = 0; i < maxSessionCount; ++i)
	{
		//shared_ptr<ChatSession> session = make_shared<ChatSession>(shared_from_this());
		shared_ptr<ChatSession> session = GetSession()();
		session->SetApp(shared_from_this());

		if (session == nullptr)
		{
			PRINT_ERROR("Create Session Error");
			return false;
		}

		if (GetIOCPHandler()->BindIOCompletionPort(session) == false)
		{
			PRINT_ERROR("IOCP Bind Error");
			return false;
		}

		if (session->Connect() == false)
			return false;

		GClientSessionManager->Add(session);	//temp
	}

	return true;
}
