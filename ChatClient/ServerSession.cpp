#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"

queue<vector<char>> GRecvPacketQueue;

void ServerSession::OnSend(unsigned int len)
{
	cout << "Client Send complete : " << len << endl;
}

void ServerSession::OnRecv(char* buffer, unsigned int len)
{
	m_buffer.resize(len);
	::memcpy(&m_buffer[0], buffer, len);
	lock_guard<recursive_mutex> lock(m_mutex);
	GRecvPacketQueue.push(m_buffer);
}

void ServerSession::OnConnect()
{
	//cout << "Connection Success" << endl;
}

void ServerSession::OnDisconnect()
{
	cout << "Disconnected Server" << endl;
}

void ServerSession::GetPacketData()
{
}
