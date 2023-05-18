#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"

unique_ptr<ServerPacketHandler> GServerPacketHandler = make_unique<ServerPacketHandler>();

ServerPacketHandler::ServerPacketHandler()
{
	m_uMapProcessPacket[PacketID::LOGIN_RESPONSE] = &PacketHandler::ProcessLogin;
	m_uMapProcessPacket[PacketID::CHAT_RESPONSE] = &PacketHandler::ProcessChat;
	//m_uMapProcessPacket[PacketID::CHAT_REQUEST] = &PacketHandler::ProcessChatResponse;
	m_uMapProcessPacket[PacketID::ROOM_ENTER_RESPONSE] = &PacketHandler::ProcessRoomEnter;
	m_uMapProcessPacket[PacketID::ROOM_LEAVE_RESPONSE] = &PacketHandler::ProcessRoomLeave;
}

ServerPacketHandler::~ServerPacketHandler()
{
	m_uMapProcessPacket.clear();
}

void ServerPacketHandler::ProcessChat(SOCKET socket, char* packetData, int size)
{
	SC_CHAT_NOTIFY chatResponse = {};
	::memcpy((char*)&chatResponse + sizeof(PACKET_HEADER), packetData, size);
	chatResponse.size += size;

	//cout << "[Chat Response Packet] Result : " << chatResponse.result << " PacketSize : " << chatResponse.size << endl;
}

void ServerPacketHandler::ProcessLogin(SOCKET socket, char* packetData, int size)
{
}

void ServerPacketHandler::ProcessRoomEnter(SOCKET socket, char* packetData, int size)
{
}

void ServerPacketHandler::ProcessRoomLeave(SOCKET socket, char* packetData, int size)
{
}

void ServerPacketHandler::ProcessChatResponse(SOCKET socket, char* packetData, int size)
{
	//SC_CHAT_REQUEST chatRequest = {};
	//::memcpy((char*)&chatRequest + PACKET_HEADER_SIZE, packetData, size);
	//chatRequest.size += size;

	//cout << "chat reponse: " << chatRequest.message << endl;
}
