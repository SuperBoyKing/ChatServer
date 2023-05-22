#include "pch.h"
#include "Room.h"
#include "RoomManager.h"
#include "ClientPacketHandler.h"

unique_ptr<ClientPacketHandler> GClientPacketHandler = make_unique<ClientPacketHandler>();

ClientPacketHandler::ClientPacketHandler()
{
	m_uMapProcessPacket[PacketID::CONNECT_REQUEST] = &ClientPacketHandler::ProcessConnect;
	m_uMapProcessPacket[PacketID::LOGIN_REQUEST] = &ClientPacketHandler::ProcessLogin;
	m_uMapProcessPacket[PacketID::CHAT_REQUEST] = &ClientPacketHandler::ProcessChat;
	m_uMapProcessPacket[PacketID::ROOM_OPEN_REQUEST] = &ClientPacketHandler::ProcessRoomOpen;
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
		itr->second(*this, socket, packet, stPacketHeader.size);	// 해당 패킷 ID 함수 호출
	}
}

void ClientPacketHandler::ProcessConnect(SOCKET socket, char* packetData, int size)
{
	int i = 0;
	SC_CONNECT_RESPONSE sendPacket = {};
	char title[6] = { "Hello" };
	GRoomManager->OpenRoom(title, 6, 10);

	for (auto& room : GRoomManager->GetRoomPool())
	{
		if (room->GetRoomNumber() != 0)
		{
			sendPacket.roomInfo[i].number = room->GetRoomNumber();
			memcpy(sendPacket.roomInfo[i].title, room->GetTitle(), strlen(room->GetTitle()));
			sendPacket.roomInfo[i].userCount = room->GetMaxUserCount();

			if (GRoomManager->GetOpenRoomCount() == ++i)
				break;
		}
	}
	sendPacket.size += (unsigned int)(sizeof(ROOM_INFO)) * i;

	SendProcessedPacket<SC_CONNECT_RESPONSE>(socket, sendPacket);
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
	CS_ROOM_OPEN_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_OPEN_REQUEST*>(packetData);
	bool isOpenSuccess = GRoomManager->OpenRoom(recvPacket->roomTitle, strlen(recvPacket->roomTitle), recvPacket->userCount);
	
	SC_ROOM_OPEN_RESPONSE sendPacket;
	sendPacket.result = isOpenSuccess;
	sendPacket.size = sizeof(SC_ROOM_OPEN_RESPONSE);
	
	SendProcessedPacket<SC_ROOM_OPEN_RESPONSE>(socket, sendPacket);
}

void ClientPacketHandler::ProcessRoomEnter(SOCKET socket, char* packetData, int size)
{
	CS_ROOM_ENTER_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_ENTER_REQUEST*>(packetData);
	SC_ROOM_ENTER_RESPONSE sendPacket;

	if (recvPacket->roomNumber > 0)
	{
		sendPacket.result = true;
	}
	else
	{
		sendPacket.result = false;
	}
	sendPacket.size = sizeof(SC_ROOM_ENTER_RESPONSE);

	SendProcessedPacket<SC_ROOM_ENTER_RESPONSE>(socket, sendPacket);
}

void ClientPacketHandler::ProcessRoomLeave(SOCKET socket, char* packetData, int size)
{
}