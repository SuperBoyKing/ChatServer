#include "pch.h"
#include "ClientPacketHandler.h"

unique_ptr<ClientPacketHandler> GClientPacketHandler = make_unique<ClientPacketHandler>();

ClientPacketHandler::ClientPacketHandler()
{
	m_uMapProcessPacket[PacketID::CONNECT_REQUEST] = &ClientPacketHandler::ProcessConnect;
	m_uMapProcessPacket[PacketID::LOGIN_REQUEST] = &ClientPacketHandler::ProcessLogin;
	m_uMapProcessPacket[PacketID::LOGIN_RESPONSE] = &ClientPacketHandler::ProcessLoginReq;
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
	packetHeader.size += sizeof(SC_ROOM_LIST_MULTIPLE) * packetHeader.packetCount;
	packetHeader.id = PacketID::CONNECT_RESPONSE;

	void* basePacketAddress = ::malloc(packetHeader.size);
	unique_ptr<SC_ROOM_LIST_MULTIPLE[]> roomInfos = make_unique<SC_ROOM_LIST_MULTIPLE[]>(packetHeader.packetCount);

	int i = 0;
	for (auto& room : GRoomManager->GetRoomObjectPool())	// RoomList 패킷 구성
	{
		if (room->GetRoomNumber() != 0 && GRoomManager->GetOpenRoomCount() != 0)
		{
			roomInfos[i].number = room->GetRoomNumber();
			::memcpy(roomInfos[i].title, room->GetTitle(), strlen(room->GetTitle()));
			roomInfos[i].currentUserCount = room->GetCurrentUserCount();
			roomInfos[i].maxUserCount = room->GetMaxUserCount();

			if (GRoomManager->GetOpenRoomCount() == i++)
				break;
		}
	}

	::memcpy(basePacketAddress, &packetHeader, PACKET_HEADER_SIZE);
	::memcpy(static_cast<char*>(basePacketAddress) + PACKET_HEADER_SIZE, roomInfos.get(), sizeof(SC_ROOM_LIST_MULTIPLE) * packetHeader.packetCount);

	SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(basePacketAddress));
	::free(basePacketAddress);
}

void ClientPacketHandler::ProcessLogin(shared_ptr<ChatSession> session, char* packetData, int size)
{
	CS_LOGIN_REQUEST* recvPacket = reinterpret_cast<CS_LOGIN_REQUEST*>(packetData);
	UINT32 length = 0;
	
	// 1. 해당 유저가 이미 로그인 되었나 여부
	if (strncmp(session->GetUserID(), recvPacket->userID, MAX_USER_ID_LENGTH) != 0)
	{
		session->SetUserID(recvPacket->userID);
		DB_LOGIN_REQUEST dbReqEvent;
		dbReqEvent.packet = *recvPacket;
		dbReqEvent.session = session;
		GDBManager->PushRequestEvent(dbReqEvent);
	}
	else
	{
		SC_LOGIN_RESPONSE sendPacket;
		sendPacket.result = false;
		SendProcessedPacket(session, &sendPacket);
	}	
}

void ClientPacketHandler::ProcessLoginReq(shared_ptr<ChatSession> session, char* packetData, int size)
{
	SC_LOGIN_RESPONSE* recvPacket = reinterpret_cast<SC_LOGIN_RESPONSE*>(packetData);
	SendProcessedPacket(session, recvPacket);
}

void ClientPacketHandler::ProcessChat(shared_ptr<ChatSession> session, char* packetData, int size)
{
	CS_CHAT_REQUEST* recvPacket = reinterpret_cast<CS_CHAT_REQUEST*>(packetData);
	SC_CHAT_RESPONSE sendPacket;
	sendPacket.result = true;
	SendProcessedPacket(session, &sendPacket);

	SC_CHAT_NOTIFY multicastPacket;
	memcpy(multicastPacket.userID, session->GetUserID(), strlen(session->GetUserID()));
	memcpy(multicastPacket.message, recvPacket->message, strlen(recvPacket->message));
	SendUserNotifyPacket(session, &multicastPacket);
}

void ClientPacketHandler::ProcessRoomOpen(shared_ptr<ChatSession> session, char* packetData, int size)
{
	CS_ROOM_OPEN_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_OPEN_REQUEST*>(packetData);

	int roomNumber = 0;
	bool isOpenSuccess = GRoomManager->OpenRoom(recvPacket->roomTitle, strlen(recvPacket->roomTitle), recvPacket->userCount, &roomNumber);

	// Open 응답 패킷 구성 및 전송
	SC_ROOM_OPEN_RESPONSE sendPacket;
	sendPacket.result = isOpenSuccess;
	sendPacket.roomNumber = roomNumber;
	session->SetRoomNumber(roomNumber);
	SendProcessedPacket(session, &sendPacket);

	// Notify 패킷을 위한 패킷 헤더 구성 
	PACKET_HEADER packetHeader;
	packetHeader.id = PacketID::ROOM_OPEN_NOTIFY;
	packetHeader.size += sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE);

	// 모든 클라이언트들에게 방이 Open되었다는 것을 전달하기 위한 Room Open Nofity 패킷 구성
	SC_ROOM_OPEN_NOTIFY_MULTIPLE broadcastPacket;
	broadcastPacket.number = roomNumber;
	memcpy(broadcastPacket.title, recvPacket->roomTitle, strlen(recvPacket->roomTitle));
	broadcastPacket.currentUserCount = 0;
	broadcastPacket.maxUserCount = recvPacket->userCount;

	// 패킷 헤더와 Room Open Nofity 패킷 조립 후 broadcast 전송
	void* baseAddress = ::malloc(sizeof(PACKET_HEADER) + sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE));
	::memcpy(baseAddress, &packetHeader, sizeof(PACKET_HEADER));
	::memcpy(static_cast<char*>(baseAddress) + sizeof(PACKET_HEADER), &broadcastPacket, sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE));
	SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(baseAddress), true);
	::free(baseAddress);
}

void ClientPacketHandler::ProcessRoomEnter(shared_ptr<ChatSession> session, char* packetData, int size)
{
	// 신규 접속한 클라이언트에게 방 접속 성공 여부를 전송할 패킷 구성
	CS_ROOM_ENTER_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_ENTER_REQUEST*>(packetData);
	shared_ptr<Room> enteredRoom = GRoomManager->SearchRoom(recvPacket->roomNumber);
	
	SC_ROOM_ENTER_RESPONSE sendPacket;
	if (enteredRoom != shared_ptr<Room>())
	{
		if (enteredRoom->Enter(session))
		{
			sendPacket.result = true;
			sendPacket.currentUserCount = enteredRoom->GetCurrentUserCount();
			session->SetRoomNumber(enteredRoom->GetRoomNumber());
		}
	}
	SendProcessedPacket(session, &sendPacket);

	if (sendPacket.result == true && enteredRoom->GetCurrentUserCount() > 1)
	{
		//  신규 접속한 클라이언트에게 RoomUserList 전송
		PACKET_HEADER packetHeader;
		packetHeader.packetCount = enteredRoom->GetCurrentUserCount() - 1; // 자기 자신은 빼준다.
		packetHeader.size += sizeof(SC_USER_LIST_NOTIFY_MULTIPLE) * packetHeader.packetCount;
		packetHeader.id = PacketID::ROOM_USER_LIST_NOTIFY;

		void* basePacketAddress = ::malloc(packetHeader.size);
		::memcpy((PACKET_HEADER*)basePacketAddress, &packetHeader, PACKET_HEADER_SIZE);

		int i = 0;
		unique_ptr<SC_USER_LIST_NOTIFY_MULTIPLE[]> userIDs = make_unique<SC_USER_LIST_NOTIFY_MULTIPLE[]>(packetHeader.packetCount);
		for (auto& user : enteredRoom->GetUserList())
		{
			if (user->GetSock() != session->GetSock())	// 자기 자신은 리스트에서 제외
				::memcpy(userIDs[i++].userID, user->GetUserID(), strlen(user->GetUserID()));
		}
		::memcpy(static_cast<char*>(basePacketAddress) + PACKET_HEADER_SIZE, userIDs.get(), sizeof(SC_USER_LIST_NOTIFY_MULTIPLE) * packetHeader.packetCount);
		SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(basePacketAddress));


		// 기존 방에 접속해있던 클라이언트들에게 신규 접속한 UserID 전송
		SC_ROOM_ENTER_USER_NOTIFY enterUserPacket;
		::memcpy(enterUserPacket.userID, session->GetUserID(), strlen(session->GetUserID()));
		SendUserNotifyPacket(session, &enterUserPacket);
	}
}

void ClientPacketHandler::ProcessRoomLeave(shared_ptr<ChatSession> session, char* packetData, int size)
{
	SC_ROOM_LEAVE_RESPONSE sendPacket;
	CS_ROOM_LEAVE_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_LEAVE_REQUEST*>(packetData);
	shared_ptr<Room> enteredRoom = GRoomManager->SearchRoom(recvPacket->roomNumber);

	if (enteredRoom != shared_ptr<Room>())
	{
		if (enteredRoom->Leave(session))
			sendPacket.result = true;
	}
	SendProcessedPacket(session, &sendPacket);

	if (sendPacket.result)
	{
		if (enteredRoom->GetCurrentUserCount() == 0)	// Room에 유저가 존재하지 않는다면 Room Close
		{
			SC_ROOM_CLOSE closePacket;
			closePacket.roomNumber = enteredRoom->GetRoomNumber();
			GRoomManager->CloseRoom(enteredRoom->GetRoomNumber());
			SendProcessedPacket(shared_ptr<ChatSession>(), &closePacket, true); // 자기 자신을 포함한 모든 유저에게 패킷 전송
		}
		else
		{
			SC_ROOM_LEAVE_USER_NOTIFY multicastPacket;
			memcpy(multicastPacket.userID, session->GetUserID(), strlen(session->GetUserID()));
			SendUserNotifyPacket(session, &multicastPacket);
		}
	}
}
