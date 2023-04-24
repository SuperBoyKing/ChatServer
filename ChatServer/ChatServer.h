#pragma once
enum class ServerState : unsigned __int8
{
	STOP,
	RUN,
	FALUT,
};

class ChatServer
{
public:
	ChatServer();
	~ChatServer();

private:
	ServerState m_state;
};

