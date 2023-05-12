#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"

void ClientSession::OnSend(unsigned int len)
{
	cout << "Server Send Complete : " << len << endl;
}

void ClientSession::OnRecv(char* buffer, unsigned int len)
{
	GClientPacketHandler->HandlePacket(GetSock(), buffer, len);
}

void ClientSession::OnConnect()
{
	GClientSessionManager->Add(static_pointer_cast<ChatSession>(shared_from_this()));
}

void ClientSession::OnDisconnect()
{
	GClientSessionManager->Remove(static_pointer_cast<ChatSession>(shared_from_this()));
}