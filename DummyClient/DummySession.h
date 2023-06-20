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
};

extern mutex queueMutex;
extern queue<int> GRecvRoomNumberQueue;

