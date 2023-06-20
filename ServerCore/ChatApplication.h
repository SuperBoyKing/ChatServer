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
/* Server에서 사용 될 자식 클래스 재정의 */
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
/* Client에서 사용 될 자식 클래스 재정의 */
//////////////////////////////////////
class ChatClient : public ChatApplication
{
public:
	ChatClient(shared_ptr<ServerAddress> serverAddress, shared_ptr<IOCPHandler> iocpHandler, SessionFactory session, int maxSessionCount = 1);
	virtual ~ChatClient();

	void SetAddress(const WCHAR* ip, const __int16 port);

	bool Start() override;

	// Packet 송신 함수 (Client -> Server)
	bool SendConnect();

	bool SendDisconnect();

	bool SendRoomList();

	bool SendRegister(const char* id, const int idSize, const char* pwd, const int pwdSize, const int sessionID = 0);

	bool SendLogin(const char* id, const int idSize, const char* pwd, const int pwdSize, const int sessionID = 0);

	bool SendLogout(const char* id, const int idSize);

	bool SendChat(const char* str, const int size , const int sessionID = 0);

	bool SendRoomOpen(const char* title, const int titleSize, const int userCount, const int sessionID = 0);

	bool SendRoomEnter(const int number, const int sessionID = 0);

	bool SendRoomLeave(const int number, const int sessionID = 0);

	void Disconnect(int numberOfThreads = 1, int sessionID = 0);

private:
	template<typename PacketType>
	bool SendPacket(PacketType& packet, int sessionID = 0)
	{
		shared_ptr<SendBuffer> sendBuf = make_shared<SendBuffer>(packet.size);
		sendBuf->CopyData((char*)&packet, packet.size);
		return m_vSessions[sessionID]->Send(sendBuf);
	}

private:
	vector<shared_ptr<ChatSession>> m_vSessions;
};