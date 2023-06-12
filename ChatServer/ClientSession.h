#pragma once
#include "ChatSession.h"

class ClientSession : public ChatSession
{
public:
	ClientSession();
	~ClientSession();

	void	OnSend(BYTE* sendData) override;
	void	OnRecv(char* buffer, unsigned int len) override;
	void	OnConnect() override;
	void	OnDisconnect() override;

private:
	unordered_map<PacketID, string> m_uMapPacketIDToString;
};

