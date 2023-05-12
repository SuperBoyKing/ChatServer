#pragma once
class ServerSession : public ChatSession
{
public:
	ServerSession() = default;
	~ServerSession() = default;

	void	OnSend(unsigned int len) override;
	void	OnRecv(char* buffer, unsigned int len) override;
	void	OnConnect() override;
	void	OnDisconnect() override;
};

