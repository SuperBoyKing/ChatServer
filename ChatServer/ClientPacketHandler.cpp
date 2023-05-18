#include "pch.h"
#include "ClientPacketHandler.h"

unique_ptr<ClientPacketHandler> GClientPacketHandler = make_unique<ClientPacketHandler>();

ClientPacketHandler::ClientPacketHandler()
{
	m_uMapProcessPacket[PacketID::LOGIN_REQUEST] = &ClientPacketHandler::ProcessLogin;
	m_uMapProcessPacket[PacketID::CHAT_REQUEST] = &ClientPacketHandler::ProcessChat;
	m_uMapProcessPacket[PacketID::ROOM_OPEN_REQUEST] = &ClientPacketHandler::ProcessRoomEnter;
	m_uMapProcessPacket[PacketID::ROOM_ENTER_REQUEST] = &ClientPacketHandler::ProcessRoomEnter;
	m_uMapProcessPacket[PacketID::ROOM_LEAVE_REQUEST] = &ClientPacketHandler::ProcessRoomLeave;
}

ClientPacketHandler::~ClientPacketHandler()
{
	m_uMapProcessPacket.clear();
}

void ClientPacketHandler::HandlePacket(SOCKET socket, char* packet)
{
	PACKET_HEADER stPacketHeader = { 0, };
	::memcpy((char*)&stPacketHeader, packet, PACKET_HEADER_SIZE);

	auto itr = m_uMapProcessPacket.find(stPacketHeader.id);
	if (itr != m_uMapProcessPacket.end())
	{
		itr->second(*this, socket, packet + PACKET_HEADER_SIZE, stPacketHeader.size - PACKET_HEADER_SIZE);	// 해당 패킷 ID 함수 호출
	}
}

void ClientPacketHandler::ProcessChat(SOCKET socket, char* packetData, int size)
{
	SC_CHAT_RESPONSE chatResponsePacket = {  };
	chatResponsePacket.result = true;
	chatResponsePacket.size += 1;

	shared_ptr<SendBuffer> packet = make_shared<SendBuffer>(chatResponsePacket.size);
	packet->CopyData(&chatResponsePacket, chatResponsePacket.size);

	if (GClientSessionManager->SendToSession(socket, packet))
	{
		SC_CHAT_NOTIFY chatRequestPacket = {};
		chatRequestPacket.size += size;
		::memcpy(chatRequestPacket.message, packetData, chatRequestPacket.size - PACKET_HEADER_SIZE);

		shared_ptr<SendBuffer> chatMsg = make_shared<SendBuffer>(chatRequestPacket.size);
		chatMsg->CopyData((char*)&chatRequestPacket, chatRequestPacket.size);
		//GClientSessionManager->SendToSession(socket, chatMsg); room send 
	}
}

void ClientPacketHandler::ProcessLogin(SOCKET socket, char* packetData, int size)
{
}

void ClientPacketHandler::ProcessRoomOpen(SOCKET socket, char* packetData, int size)
{

}

void ClientPacketHandler::ProcessRoomEnter(SOCKET socket, char* packetData, int size)
{
}

void ClientPacketHandler::ProcessRoomLeave(SOCKET socket, char* packetData, int size)
{
}