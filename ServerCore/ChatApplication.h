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
	ChatClient(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount = 1);
	virtual ~ChatClient();

	void Connect(const WCHAR* ip, const __int16 port);

	bool Start() override;

	void SendLogin(const char* id, const char* pwd, const int idSize, const int pwdSize);

	void SendChat(const char* str, const int size);

	inline ChatSession* GetChatSession() { return m_session.get(); }

private:
	shared_ptr<ChatSession> m_session;
};

