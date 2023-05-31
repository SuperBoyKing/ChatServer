#pragma once
#include "CRedisConn.h"

class DBManager
{
public:
	DBManager() = default;
	~DBManager() = default;

	void Run(string ip, UINT16 port);
	void Disconnect();

private:
	RedisCpp::CRedisConn m_conn;
};

