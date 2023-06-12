#pragma once
class ServerSession : public ChatSession
{
public:
	ServerSession() = default;
	~ServerSession() = default;

	void	OnSend(BYTE* packetHeader) override;
	void	OnRecv(char* buffer, unsigned int len) override;
	void	OnConnect() override;
	void	OnDisconnect() override;

	void	GetPacketData();

private:
	recursive_mutex	m_mutex;
	vector<char>	m_buffer;
};

extern queue<vector<char>> GRecvPacketQueue;

