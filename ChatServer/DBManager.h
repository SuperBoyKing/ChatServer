#pragma once
#include "CRedisConn.h"

struct DB_LOGIN_REQUEST
{
	CS_LOGIN_REQUEST packet;
	shared_ptr<ChatSession> session;
};

struct DB_LOGIN_RESPONSE
{
	SC_LOGIN_RESPONSE packet;
	shared_ptr<ChatSession> session;
};

class DBManager
{
public:
	DBManager() = default;
	~DBManager() = default;

	void Run(string ip, UINT16 port, int threadCount = 1);
	void Disconnect();
	bool insert(const char* id, const char* pw);

private:
	void ProcesssDBEvent();
	void PushReponseEvent(DB_LOGIN_RESPONSE& loginResponseEvent);
	DB_LOGIN_REQUEST TakeOutRequestEvent();
	void CallPQCS();

public:
	void PushRequestEvent(DB_LOGIN_REQUEST& loginRequestEvent);
	DB_LOGIN_RESPONSE TakeOutResponseEvent();
	bool Search(string userID);
	inline void SetIOCPHandle(HANDLE iocpHandle) { m_iocpHandle = iocpHandle; }

private:
	RedisCpp::CRedisConn m_conn;

	// event deque
	deque<DB_LOGIN_REQUEST> m_requestEventDeque;
	deque<DB_LOGIN_RESPONSE> m_responseEventDeque;

	// loop boolean, mutex etc...
	bool m_isEventLoop = false;
	bool m_isPQCSLoop;
	mutex m_requestMutex;
	mutex m_responseMutex;
	mutex m_getMutex;
	ThreadManager m_DBThreadManager;
	ThreadManager m_PQCSThreadManager;
	DBResOperation m_dbEventOperation;

	HANDLE m_iocpHandle;
};

extern unique_ptr<DBManager> GDBManager;