#include "pch.h"
#include "DummySession.h"

queue<int> GRecvRoomNumberQueue;
mutex queueMutex;

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
	SC_ROOM_OPEN_RESPONSE* recvPacket = reinterpret_cast<SC_ROOM_OPEN_RESPONSE*>(buffer);
	if (recvPacket->id == PacketID::ROOM_OPEN_RESPONSE)
	{
		lock_guard<mutex> lock(queueMutex);
		GRecvRoomNumberQueue.push(recvPacket->roomNumber);
	}
}

void DummySession::OnConnect()
{
	GClientSessionManager->Add(static_pointer_cast<ChatSession>(shared_from_this()));
}

void DummySession::OnDisconnect()
{
	GClientSessionManager->Remove(static_pointer_cast<ChatSession>(shared_from_this()));
}
