#include "pch.h"
#include "ClientPacketHandler.h"

unique_ptr<ClientPacketHandler> GClientPacketHandler = make_unique<ClientPacketHandler>();

ClientPacketHandler::ClientPacketHandler()
{
	m_uMapProcessPacket[PacketID::LOGIN_REQUEST] = &PacketHandler::ProcessLogin;
	m_uMapProcessPacket[PacketID::CHAT_REQUEST] = &PacketHandler::ProcessChat;
	m_uMapProcessPacket[PacketID::ROOM_ENTER_REQUEST] = &PacketHandler::ProcessRoomEnter;
	m_uMapProcessPacket[PacketID::ROOM_LEAVE_REQUEST] = &PacketHandler::ProcessRoomLeave;
}

ClientPacketHandler::~ClientPacketHandler()
{
	m_uMapProcessPacket.clear();
}

void ClientPacketHandler::ProcessChat(SOCKET socket, char* packetData, int size)
{
	/*SC_CHAT_RESPONSE chatResponsePacket = {  };
	chatResponsePacket.result = true;
	chatResponsePacket.size += 1;

	shared_ptr<SendBuffer> packet = make_shared<SendBuffer>(chatResponsePacket.size);
	packet->CopyData(&chatResponsePacket, chatResponsePacket.size);

	if (GClientSessionManager->SendToSession(socket, packet))
	{
	}*/

	SC_CHAT_RESPONSE chatRequestPacket = {};
	chatRequestPacket.size += size;
	::memcpy(chatRequestPacket.message, packetData, chatRequestPacket.size - PACKET_HEADER_SIZE);

	shared_ptr<SendBuffer> chatMsg = make_shared<SendBuffer>(chatRequestPacket.size);
	chatMsg->CopyData((char*)&chatRequestPacket, chatRequestPacket.size);
	GClientSessionManager->SendToSession(socket, chatMsg);
	//GClientSessionManager->Broadcast(chatMsg);
}

void ClientPacketHandler::ProcessLogin(SOCKET socket, char* packetData, int size)
{
}

void ClientPacketHandler::ProcessRoomEnter(SOCKET socket, char* packetData, int size)
{
}

void ClientPacketHandler::ProcessRoomLeave(SOCKET socket, char* packetData, int size)
{
}