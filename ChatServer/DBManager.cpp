#include "pch.h"
#include "DBManager.h"

void DBManager::Run(string ip, UINT16 port)
{
	if (m_conn.connect(ip, port))
	{
		std::cout << "Database connection success!" << endl;
	}
	else
	{
		std::cout << "Database connection error : " << m_conn.getErrorStr() << endl;
	}

	// db task thread 추가 필요
}

void DBManager::Disconnect()
{
	m_conn.disConnect();
}
