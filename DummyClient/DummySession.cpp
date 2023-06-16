#include "pch.h"
#include "DummySession.h"

queue<vector<char>> GRecvPacketQueue;

void DummySession::OnSend(BYTE* packet)
{
	PACKET_HEADER* packetHeader = reinterpret_cast<PACKET_HEADER*>(packet);
	if (packetHeader->id == PacketID::CHAT_REQUEST)
		cout << "Dummy Client Send Chat complete : " << reinterpret_cast<CS_CHAT_REQUEST*>(packet)->message << endl;
	else
		cout << "Dummy Client Send complete" << endl;
}

void DummySession::OnRecv(char* buffer, unsigned int len)
{
	m_buffer.resize(len);
	::memcpy(&m_buffer[0], buffer, len);
	lock_guard<recursive_mutex> lock(m_mutex);
	GRecvPacketQueue.push(m_buffer);
}

void DummySession::OnConnect()
{
	GClientSessionManager->Add(static_pointer_cast<ChatSession>(shared_from_this()));
}

void DummySession::OnDisconnect()
{
	GClientSessionManager->Remove(static_pointer_cast<ChatSession>(shared_from_this()));
}

void DummySession::GetPacketData()
{
}
