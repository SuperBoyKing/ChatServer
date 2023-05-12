#pragma once
class ChatSession;
class ClientListener;

using SessionFactory = function<shared_ptr<ChatSession>(void)>;

enum class ApplicationType : unsigned __int8
{
	SERVER,
	CLIENT
};

class ChatApplication : public enable_shared_from_this<ChatApplication>
{
public:
	ChatApplication(ApplicationType appType, shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount);
	virtual ~ChatApplication();

	virtual bool Start() abstract;

	inline ApplicationType				GetApplicationType() const	{ return m_appType; }
	inline shared_ptr<ServerAddress>	GetAddress() const			{ return m_serverAddress; }
	inline shared_ptr<IOCPHandler>		GetIOCPHandler() const		{ return m_iocpHandler; }
	inline SessionFactory				GetSession() const			{ return m_sessionFactory; }
	inline int							GetMaxSessionCount() const	{ return m_maxSessionCount; }

protected:
	ApplicationType				m_appType;
	shared_ptr<ServerAddress>	m_serverAddress;
	shared_ptr<IOCPHandler>		m_iocpHandler;
	SessionFactory				m_sessionFactory;
	int							m_maxSessionCount;
};

class ChatServer : public ChatApplication
{
public:
	ChatServer(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount);
	virtual ~ChatServer();

	bool Start() override;

private:
	shared_ptr<ClientListener>	m_clientListener;
};

class ChatClient : public ChatApplication
{
public:
	ChatClient(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount);
	virtual ~ChatClient();

	bool Start() override;

	void Send(shared_ptr<SendBuffer> sendBuffer);
};

