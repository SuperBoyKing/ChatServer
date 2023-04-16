#include "pch.h"
#include "ServerAddress.h"

ServerAddress::ServerAddress()
{
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_serverAddr.sin_port = htons(SERVER_PORT);
}

ServerAddress::ServerAddress(SOCKADDR_IN serverAddr)
	: m_serverAddr(serverAddr)
{

}

ServerAddress::ServerAddress(const WCHAR* ip, const __int16 port)
{
	IN_ADDR address;
	::InetPton(AF_INET, ip, &address);

	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr = address;
	m_serverAddr.sin_port = htons(port);
}