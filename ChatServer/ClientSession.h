#pragma once
#include "ChatSession.h"

class ClientSession : public ChatSession
{
public:
	void	OnSend(unsigned int len) override;
	void	OnRecv(char* buffer, unsigned int len) override;
	void	OnConnect() override;
	void	OnDisconnect() override;
};

