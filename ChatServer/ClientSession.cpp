#include "pch.h"
#include "ClientSession.h"

void ClientSession::OnSend(unsigned int len)
{

}

void ClientSession::OnRecv(char* buffer, unsigned int len)
{

}

void ClientSession::OnConnect()
{
	GClientSessionManager->Add(static_pointer_cast<ChatSession>(shared_from_this()));
}

void ClientSession::OnDisconnect()
{
	GClientSessionManager->Remove(static_pointer_cast<ChatSession>(shared_from_this()));
}

void ClientSession::RecvPacket()
{

}
