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

void ClientPacketHandler::HandlePacket(shared_ptr<ChatSession> session, char* packet)
{
	PACKET_HEADER stPacketHeader = { 0, };
	::memcpy((char*)&stPacketHeader, packet, PACKET_HEADER_SIZE);

	auto itr = m_uMapProcessPacket.find(stPacketHeader.id);
	if (itr != m_uMapProcessPacket.end())
	{
		itr->second(*this, session, packet, stPacketHeader.size);	// 해당 패킷 ID 함수 호출
	}
}

void ClientPacketHandler::ProcessConnect(shared_ptr<ChatSession> session, char* packetData, int size)
{
	PACKET_HEADER packetHeader;
	packetHeader.packetCount = GRoomManager->GetOpenRoomCount();
	packetHeader.size += sizeof(SC_CONNECT_RESPONSE) * packetHeader.packetCount;
	packetHeader.id = PacketID::CONNECT_RESPONSE;

	void* basePacketAddress = ::malloc(packetHeader.size);
	unique_ptr<SC_CONNECT_RESPONSE[]> roomInfos = make_unique<SC_CONNECT_RESPONSE[]>(packetHeader.packetCount);

	int i = 0;
	for (auto& room : GRoomManager->GetRoomPool())
	{
		if (room->GetRoomNumber() != 0 && GRoomManager->GetOpenRoomCount() != 0)
		{
			roomInfos[i].number = room->GetRoomNumber();
			::memcpy(roomInfos[i].title, room->GetTitle(), strlen(room->GetTitle()));
			roomInfos[i].userCount = room->GetMaxUserCount();

			if (GRoomManager->GetOpenRoomCount() == i++)
				break;
		}
	}

	::memcpy(basePacketAddress, &packetHeader, PACKET_HEADER_SIZE);
	::memcpy(static_cast<char*>(basePacketAddress) + PACKET_HEADER_SIZE, roomInfos.get(), sizeof(SC_CONNECT_RESPONSE) * packetHeader.packetCount);

	SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(basePacketAddress));
	::free(basePacketAddress);
}

void ClientPacketHandler::ProcessLogin(shared_ptr<ChatSession> session, char* packetData, int size)
{
}

void ClientPacketHandler::ProcessChat(shared_ptr<ChatSession> session, char* packetData, int size)
{
}

void ClientPacketHandler::ProcessRoomOpen(shared_ptr<ChatSession> session, char* packetData, int size)
{
	CS_ROOM_OPEN_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_OPEN_REQUEST*>(packetData);

	int roomNumber = 0;
	bool isOpenSuccess = GRoomManager->OpenRoom(recvPacket->roomTitle, strlen(recvPacket->roomTitle), recvPacket->userCount, &roomNumber);

	SC_ROOM_OPEN_RESPONSE sendPacket;
	sendPacket.result = isOpenSuccess;
	sendPacket.roomNumber = roomNumber;
	sendPacket.size = sizeof(SC_ROOM_OPEN_RESPONSE);
	SendProcessedPacket(session, &sendPacket);

	PACKET_HEADER packetHeader;
	packetHeader.id = PacketID::ROOM_OPEN_NOTIFY;
	packetHeader.size += sizeof(SC_ROOM_OPEN_NOTIFY);

	SC_ROOM_OPEN_NOTIFY broadcastPacket;
	broadcastPacket.number = roomNumber;
	memcpy(broadcastPacket.title, recvPacket->roomTitle, strlen(recvPacket->roomTitle));
	broadcastPacket.userCount = recvPacket->userCount;

	void* baseAddress = ::malloc(sizeof(PACKET_HEADER) + sizeof(SC_ROOM_OPEN_NOTIFY));
	::memcpy(baseAddress, &packetHeader, sizeof(PACKET_HEADER));
	::memcpy(static_cast<char*>(baseAddress) + sizeof(PACKET_HEADER), &broadcastPacket, sizeof(SC_ROOM_OPEN_NOTIFY));
	SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(baseAddress), true);
	::free(baseAddress);
}

void ClientPacketHandler::ProcessRoomEnter(shared_ptr<ChatSession> session, char* packetData, int size)
{
	/*/////////////////////////////////////////////////////
		  방에 접속한 클라이언트에게 방 접속 성공 여부 전송
	/////////////////////////////////////////////////////*/
	CS_ROOM_ENTER_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_ENTER_REQUEST*>(packetData);
	shared_ptr<Room> enteredRoom = GRoomManager->SearchRoom(recvPacket->roomNumber - 1);
	
	SC_ROOM_ENTER_RESPONSE sendPacket;
	sendPacket.size = sizeof(SC_ROOM_ENTER_RESPONSE);
	
	if (enteredRoom != shared_ptr<Room>())
	{
		if (enteredRoom->Enter(session))
			sendPacket.result = true;
	}
	SendProcessedPacket(session, &sendPacket);

	if (enteredRoom->GetCurrentUserCount() > 1 && sendPacket.result == true)
	{
		session->SetUserID("TestClient");


		///*/////////////////////////////////////////////////////
		//  방에 접속한 클라이언트에게 RoomUserList 전송
		///////////////////////////////////////////////////////*/
		PACKET_HEADER packetHeader;
		packetHeader.packetCount = enteredRoom->GetCurrentUserCount() - 1; // 자기 자신은 빼준다.
		packetHeader.size += sizeof(SC_ROOM_USERLIST_NOTIFY) * packetHeader.packetCount;
		packetHeader.id = PacketID::ROOM_USER_LIST_NOTIFY;

		void* basePacketAddress = ::malloc(packetHeader.size);
		::memcpy((PACKET_HEADER*)basePacketAddress, &packetHeader, PACKET_HEADER_SIZE);

		unique_ptr<SC_ROOM_USERLIST_NOTIFY[]> userIDs = make_unique<SC_ROOM_USERLIST_NOTIFY[]>(packetHeader.packetCount);
		int i = 0;
		for (auto& user : enteredRoom->GetUserList())
		{
			if (user->GetSock() != session->GetSock())	// 자기 자신은 리스트에서 제외
				::memcpy(userIDs[i++].userID, session->GetUserID(), strlen(session->GetUserID()));
		}
		::memcpy(static_cast<char*>(basePacketAddress) + PACKET_HEADER_SIZE, userIDs.get(), sizeof(SC_ROOM_USERLIST_NOTIFY) * packetHeader.packetCount);
		SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(basePacketAddress));


		/*/////////////////////////////////////////////////////
			기존 방에 접속해있던 클라이언트들에게 신규 접속한 UserID 전송
		/////////////////////////////////////////////////////*/
		for (auto& user : enteredRoom->GetUserList())
		{
			if (user->GetSock() != session->GetSock())	// 자기 자신은 리스트에서 제외
			{
				SC_ROOM_ENTER_USER_NOTIFY enterUserPacket;
				::memcpy(enterUserPacket.userID, session->GetUserID(), strlen(session->GetUserID()));
				enterUserPacket.size = sizeof(SC_ROOM_ENTER_USER_NOTIFY);
				SendProcessedPacket(user, &enterUserPacket);
			}
		}
		
	}
}

void ClientPacketHandler::ProcessRoomLeave(shared_ptr<ChatSession> session, char* packetData, int size)
{
	SC_ROOM_LEAVE_RESPONSE sendPacket;
	CS_ROOM_LEAVE_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_LEAVE_REQUEST*>(packetData);
	shared_ptr<Room> enteredRoom = GRoomManager->SearchRoom(recvPacket->roomNumber - 1);

	if (enteredRoom != shared_ptr<Room>())
	{
		if (enteredRoom->Leave(session))
			sendPacket.result = true;
	}
	SendProcessedPacket(session, &sendPacket);

	if (sendPacket.result)
	{
		SC_ROOM_LEAVE_NOTIFY broadcastPacket;
		
	}
}