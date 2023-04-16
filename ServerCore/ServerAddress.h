#pragma once
class ServerAddress
{
public:
	ServerAddress();
	ServerAddress(SOCKADDR_IN serverAddr);
	ServerAddress(const WCHAR* ip, const __int16 port);
	~ServerAddress() = default;

	SOCKADDR_IN GetSockAddrIn() const { return m_serverAddr; }

private:
	SOCKADDR_IN m_serverAddr;
};

