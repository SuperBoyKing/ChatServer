#pragma once
#include "ClientSession.h"

class ChatSession : public ClientSession
{
public:
	ChatSession();
	virtual ~ChatSession();

	void	OnSend(unsigned int len) override;
	void	OnRecv(char* buffer, unsigned int len) override;
	void	OnConnect() override;
	void	OnDisconnect() override;

private:

};

