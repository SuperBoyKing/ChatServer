#pragma once
class DummySession : public ChatSession
{
public:
	DummySession() = default;
	~DummySession() = default;

	void	OnSend(BYTE* len) override;
	void	OnRecv(char* buffer, unsigned int len) override;
	void	OnConnect() override;
	void	OnDisconnect() override;

	void	GetPacketData();

private:
	recursive_mutex	m_mutex;
	vector<char>	m_buffer;
};

extern queue<vector<char>> GRecvPacketQueue;

