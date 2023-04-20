#pragma once
class ClientSession
{
public:
	ClientSession();
	~ClientSession();

private:
	SOCKET m_socket;
};

