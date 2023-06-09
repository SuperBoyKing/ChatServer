#include "pch.h"
#include "ClientPacketHandler.h"

unique_ptr<ClientPacketHandler> GClientPacketHandler = make_unique<ClientPacketHandler>();

ClientPacketHandler::ClientPacketHandler()
{
	m_uMapProcessPacket[PacketID::CONNECT_REQUEST] = &ClientPacketHandler::ProcessConnect;
	m_uMapProcessPacket[PacketID::DISCONNECT_REQUEST] = &ClientPacketHandler::ProcessDisconnect;
	m_uMapProcessPacket[PacketID::ROOM_LIST_REQUEST] = &ClientPacketHandler::ProcessRoomList;
	m_uMapProcessPacket[PacketID::LOGIN_REQUEST] = &ClientPacketHandler::ProcessLogin;
	m_uMapProcessPacket[PacketID::LOGIN_RESPONSE] = &ClientPacketHandler::ProcessLoginReq;
	m_uMapProcessPacket[PacketID::LOGOUT_REQUEST] = &ClientPacketHandler::ProcessLogout;
	m_uMapProcessPacket[PacketID::REGISTER_REQUEST] = &ClientPacketHandler::ProcessResgister;
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
	CS_CONNECT_REQUEST* recvPacket = reinterpret_cast<CS_CONNECT_REQUEST*>(packetData);
	SC_CONNECT_RESPONSE sendPacket;
	sendPacket.result = true;

	SendProcessedPacket(session, &sendPacket);
}

void ClientPacketHandler::ProcessDisconnect(shared_ptr<ChatSession> session, char* packetData, int size)
{
	if (session->GetSessionState() == SessionState::ROOM)
	{
		shared_ptr<Room> enteredRoom = GRoomManager->SearchRoom(session->GetRoomNumber());
		SC_ROOM_LEAVE_USER_NOTIFY multicastPacket;
		memcpy(multicastPacket.userID, session->GetUserID(), strlen(session->GetUserID()) + 1);
		enteredRoom->SendUserNotifyPacket(session, &multicastPacket);
	}
}	

void ClientPacketHandler::ProcessRoomList(shared_ptr<ChatSession> session, char* packetData, int size)
{
	PACKET_HEADER packetHeader;
	packetHeader.packetCount = GRoomManager->GetOpenRoomCount();
	packetHeader.size += sizeof(SC_ROOM_LIST_MULTIPLE) * packetHeader.packetCount;
	packetHeader.id = PacketID::ROOM_LIST_RESPONSE;

	void* basePacketAddress = ::malloc(packetHeader.size);
	unique_ptr<SC_ROOM_LIST_MULTIPLE[]> roomInfos = make_unique<SC_ROOM_LIST_MULTIPLE[]>(packetHeader.packetCount);

	int i = 0;
	for (auto& room : GRoomManager->GetRoomObjectPool())	// RoomList 패킷 구성
	{
		if (room->GetRoomNumber() != 0 && GRoomManager->GetOpenRoomCount() != 0)
		{
			roomInfos[i].number = room->GetRoomNumber();
			::memcpy(roomInfos[i].title, room->GetTitle(), strlen(room->GetTitle()) + 1);
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

	// 해당 유저가 이미 로그인 되었나 여부
	if (session->GetSessionState() == SessionState::NONE)
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
	if (recvPacket->result == true)
	{
		session->SetSessionState(SessionState::LOGIN);
	}
	else
	{
		session->SetUserID("");
		session->SetSessionState(SessionState::NONE);
	}
	SendProcessedPacket(session, recvPacket);
}

void ClientPacketHandler::ProcessLogout(shared_ptr<ChatSession> session, char* packetData, int size)
{
	CS_LOGOUT_REQUEST* recvPacket = reinterpret_cast<CS_LOGOUT_REQUEST*>(packetData);
	SC_LOGOUT_RESPONSE sendPacket;
	
	if (session->GetSessionState() == SessionState::LOGIN)
	{
		sendPacket.result = true;
		session->SetUserID("");
		session->SetSessionState(SessionState::NONE);
	}

	SendProcessedPacket(session, &sendPacket);
}

void ClientPacketHandler::ProcessResgister(shared_ptr<ChatSession> session, char* packetData, int size)
{
	CS_RESGISTER_REQUEST* recvPacket = reinterpret_cast<CS_RESGISTER_REQUEST*>(packetData);
	SC_RESGISTER_RESPONSE sendPacket;

	if (session->GetSessionState() == SessionState::NONE && GDBManager->Search(recvPacket->userID) == false)
	{
		GDBManager->insert(recvPacket->userID, recvPacket->userPW);
		sendPacket.result = true;
	}

	SendProcessedPacket(session, &sendPacket);
}

void ClientPacketHandler::ProcessChat(shared_ptr<ChatSession> session, char* packetData, int size)
{
	CS_CHAT_REQUEST* recvPacket = reinterpret_cast<CS_CHAT_REQUEST*>(packetData);
	SC_CHAT_RESPONSE sendPacket;
	sendPacket.result = true;
	SendProcessedPacket(session, &sendPacket);

	SC_CHAT_NOTIFY multicastPacket;
	memcpy(multicastPacket.userID, session->GetUserID(), strlen(session->GetUserID()) + 1);
	memcpy(multicastPacket.message, recvPacket->message, strlen(recvPacket->message) + 1);
	shared_ptr<Room> enteredRoom = GRoomManager->SearchRoom(session->GetRoomNumber());

	if (enteredRoom != shared_ptr<Room>() && enteredRoom.get()->GetRoomNumber() != 0)
		enteredRoom->SendUserNotifyPacket(session, &multicastPacket);
}

void ClientPacketHandler::ProcessRoomOpen(shared_ptr<ChatSession> session, char* packetData, int size)
{
	CS_ROOM_OPEN_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_OPEN_REQUEST*>(packetData);

	int roomNumber = 0;
	bool isOpenSuccess = GRoomManager->OpenRoom(recvPacket->roomTitle, strlen(recvPacket->roomTitle) + 1, recvPacket->userCount, &roomNumber);

	// Open 응답 패킷 구성 및 전송
	SC_ROOM_OPEN_RESPONSE sendPacket;
	sendPacket.result = isOpenSuccess;
	sendPacket.roomNumber = roomNumber;
	SendProcessedPacket(session, &sendPacket);

	if (isOpenSuccess)
	{
		// Notify 패킷을 위한 패킷 헤더 구성 
		PACKET_HEADER packetHeader;
		packetHeader.id = PacketID::ROOM_OPEN_NOTIFY;
		packetHeader.size += sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE);

		// 모든 클라이언트들에게 방이 Open되었다는 것을 전달하기 위한 Room Open Nofity 패킷 구성
		SC_ROOM_OPEN_NOTIFY_MULTIPLE broadcastPacket;
		broadcastPacket.number = roomNumber;
		memcpy(broadcastPacket.title, recvPacket->roomTitle, strlen(recvPacket->roomTitle) + 1);
		broadcastPacket.currentUserCount = 0;
		broadcastPacket.maxUserCount = recvPacket->userCount;

		// 패킷 헤더와 Room Open Nofity 패킷 조립 후 broadcast 전송
		void* baseAddress = ::malloc(sizeof(PACKET_HEADER) + sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE));
		::memcpy(baseAddress, &packetHeader, sizeof(PACKET_HEADER));
		::memcpy(static_cast<char*>(baseAddress) + sizeof(PACKET_HEADER), &broadcastPacket, sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE));
		SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(baseAddress), true);
		::free(baseAddress);
	}
}

void ClientPacketHandler::ProcessRoomEnter(shared_ptr<ChatSession> session, char* packetData, int size)
{
	// 신규 접속한 클라이언트에게 방 접속 성공 여부를 전송할 패킷 구성
	CS_ROOM_ENTER_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_ENTER_REQUEST*>(packetData);
	shared_ptr<Room> enteredRoom = GRoomManager->SearchRoom(recvPacket->roomNumber);
	
	SC_ROOM_ENTER_RESPONSE sendPacket;
	if (enteredRoom != shared_ptr<Room>() && enteredRoom.get()->GetRoomNumber() != 0)
	{
		if (session->GetSessionState() == SessionState::LOGIN && enteredRoom->Enter(session))
		{
			sendPacket.result = true;
			session->SetRoomNumber(enteredRoom->GetRoomNumber());
			session->SetSessionState(SessionState::ROOM);
		}
		else
		{
			sendPacket.result = false;
		}
	}
	SendProcessedPacket(session, &sendPacket);

	if (sendPacket.result == true) // 방 접속에 성공 할 경우
	{
		PACKET_HEADER packetHeader;
		packetHeader.packetCount = enteredRoom->GetCurrentUserCount();
		packetHeader.size += sizeof(SC_USER_LIST_NOTIFY_MULTIPLE) * packetHeader.packetCount;
		packetHeader.id = PacketID::ROOM_USER_LIST_NOTIFY;

		void* basePacketAddress = malloc(packetHeader.size);
		::memcpy(basePacketAddress, &packetHeader, PACKET_HEADER_SIZE);

		enteredRoom->MakeUserListPacket(OUT basePacketAddress, IN packetHeader.packetCount);

		//  신규 접속한 클라이언트에게 RoomUserList 전송
		SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(basePacketAddress));
		::free(basePacketAddress);
		
		// 기존 방에 접속해있던 클라이언트들에게 신규 접속한 UserID 전송
		SC_ROOM_ENTER_USER_NOTIFY enterUserPacket;
		::memcpy(enterUserPacket.userID, session->GetUserID(), strlen(session->GetUserID()) + 1);
		enteredRoom->SendUserNotifyPacket(session, &enterUserPacket);
	}
}

void ClientPacketHandler::ProcessRoomLeave(shared_ptr<ChatSession> session, char* packetData, int size)
{
	SC_ROOM_LEAVE_RESPONSE sendPacket;
	CS_ROOM_LEAVE_REQUEST* recvPacket = reinterpret_cast<CS_ROOM_LEAVE_REQUEST*>(packetData);
	shared_ptr<Room> enteredRoom = GRoomManager->SearchRoom(recvPacket->roomNumber);

	if (enteredRoom != shared_ptr<Room>())
	{
		if (session->GetSessionState() == SessionState::ROOM && enteredRoom->Leave(session))
		{
			sendPacket.result = true;
			session->SetSessionState(SessionState::LOGIN);
			session->SetRoomNumber(0); // Leave가 성공한 경우, 해당 세션이 가지고 있는 방 번호를 초기화한다.
		}
		else
		{
			sendPacket.result = false;
		}
	}

	if (size != 0)	// size가 0이라면 강제 종료된 세션이므로 응답메시지를 송신하지 않는다.
		SendProcessedPacket(session, &sendPacket);

	if (sendPacket.result)
	{
		if (enteredRoom->GetCurrentUserCount() == 0)	// Room에 유저가 존재하지 않는다면 Room Close
		{
			SC_ROOM_CLOSE closePacket;
			closePacket.roomNumber = enteredRoom->GetRoomNumber();
			GRoomManager->CloseRoom(closePacket.roomNumber);

			if (size != 0)
				SendProcessedPacket(shared_ptr<ChatSession>(), &closePacket, true); // 자기 자신을 포함한 모든 유저에게 패킷 전송
			else
				SendProcessedPacket(session, &closePacket, true); // size가 0이라면 강제 종료된 세션이므로 자신을 제외한 모든 유저에게 패킷 전송
		}
		else
		{
			SC_ROOM_LEAVE_USER_NOTIFY multicastPacket;
			memcpy(multicastPacket.userID, session->GetUserID(), strlen(session->GetUserID()) + 1);
			enteredRoom->SendUserNotifyPacket(session, &multicastPacket);
		}
	}
}
