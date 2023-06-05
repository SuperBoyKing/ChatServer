#pragma once
class ChatSession;
class ClientListener;

using SessionFactory = function<shared_ptr<ChatSession>(void)>;

enum class SessionType : unsigned __int8
{
	SERVER,
	CLIENT
};

class ChatApplication : public enable_shared_from_this<ChatApplication>
{
public:
	ChatApplication(SessionType appType, shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount);
	virtual ~ChatApplication();

	virtual bool Start() abstract;

	inline SessionType					GetSessionType() const		{ return m_appType; }
	inline shared_ptr<ServerAddress>	GetAddress() const			{ return m_serverAddress; }
	inline shared_ptr<IOCPHandler>		GetIOCPHandler() const		{ return m_iocpHandler; }
	inline SessionFactory				GetSession() const			{ return m_sessionFactory; }
	inline int							GetMaxSessionCount() const	{ return m_maxSessionCount; }

protected:
	SessionType					m_appType;
	shared_ptr<ServerAddress>	m_serverAddress;
	shared_ptr<IOCPHandler>		m_iocpHandler;
	SessionFactory				m_sessionFactory;
	int							m_maxSessionCount;
};

//////////////////////////////////////
/* Server���� ��� �� �ڽ� Ŭ���� ������ */
//////////////////////////////////////
class ChatServer : public ChatApplication
{
public:
	ChatServer(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount);
	virtual ~ChatServer();

	bool Start() override;

private:
	shared_ptr<ClientListener>	m_clientListener;
};

//////////////////////////////////////
/* Client���� ��� �� �ڽ� Ŭ���� ������ */
//////////////////////////////////////
class ChatClient : public ChatApplication
{
public:
	ChatClient(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount = 1);
	virtual ~ChatClient();

	void SetAddress(const WCHAR* ip, const __int16 port);

	bool Start() override;

	// Packet �۽� �Լ� (Client -> Server)
	void SendConnect();

	void SendRegister(const char* id, const int idSize, const char* pwd, const int pwdSize);

	void SendLogin(const char* id, const int idSize, const char* pwd, const int pwdSize);

	void SendLogout(const char* id, const int idSize);

	void SendChat(const char* str, const int size);

	void SendRoomOpen(char* title, int titleSize, int userCount);

	void SendRoomEnter(int number);

	void SendRoomLeave(int number);

	void Disconnect(int numberOfThreads = 1);

private:
	template<typename PacketType>
	void SendPacket(PacketType& packet)
	{
		shared_ptr<SendBuffer> sendBuf = make_shared<SendBuffer>(packet.size);
		sendBuf->CopyData((char*)&packet, packet.size);
		m_session->Send(sendBuf);
	}

private:
	shared_ptr<ChatSession> m_session;
};