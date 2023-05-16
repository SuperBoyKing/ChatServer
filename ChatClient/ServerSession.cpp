#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"

void ServerSession::OnSend(unsigned int len)
{
	cout << "Client Send complete : " << len << endl;
}

void ServerSession::OnRecv(char* buffer, unsigned int len)
{
	GServerPacketHandler->HandlePacket(GetSock(), buffer, len);
	//m_recvPacketQueue.push()
}

void ServerSession::OnConnect()
{
	cout << "Connection Success" << endl;
}

void ServerSession::OnDisconnect()
{
	cout << "Disconnected Server" << endl;
}

void ServerSession::GetPacketData()
{
}
