#pragma once
class ServerAddress
{
public:
	ServerAddress();
	ServerAddress(SOCKADDR_IN serverAddr);
	ServerAddress(const WCHAR* ip, const __int16 port);
	~ServerAddress() = default;

	void SetServerAddress(const WCHAR* ip, const __int16 port);

	SOCKADDR_IN GetSockAddrIn() const { return m_serverAddr; }

	wstring						GetIP();
	inline const unsigned short	GetPort() { return ::ntohs(m_serverAddr.sin_port); }

private:
	SOCKADDR_IN m_serverAddr = {};
};

