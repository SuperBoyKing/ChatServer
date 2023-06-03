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
		itr->second(*this, session, packet, stPacketHeader.size);	// �ش� ��Ŷ ID �Լ� ȣ��
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
	for (auto& room : GRoomManager->GetRoomObjectPool())	// RoomList ��Ŷ ����
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
	
	// 1. �ش� ������ �̹� �α��� �Ǿ��� ����
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

	// Open ���� ��Ŷ ���� �� ����
	SC_ROOM_OPEN_RESPONSE sendPacket;
	sendPacket.result = isOpenSuccess;
	sendPacket.roomNumber = roomNumber;
	session->SetRoomNumber(roomNumber);
	SendProcessedPacket(session, &sendPacket);

	// Notify ��Ŷ�� ���� ��Ŷ ��� ���� 
	PACKET_HEADER packetHeader;
	packetHeader.id = PacketID::ROOM_OPEN_NOTIFY;
	packetHeader.size += sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE);

	// ��� Ŭ���̾�Ʈ�鿡�� ���� Open�Ǿ��ٴ� ���� �����ϱ� ���� Room Open Nofity ��Ŷ ����
	SC_ROOM_OPEN_NOTIFY_MULTIPLE broadcastPacket;
	broadcastPacket.number = roomNumber;
	memcpy(broadcastPacket.title, recvPacket->roomTitle, strlen(recvPacket->roomTitle));
	broadcastPacket.currentUserCount = 0;
	broadcastPacket.maxUserCount = recvPacket->userCount;

	// ��Ŷ ����� Room Open Nofity ��Ŷ ���� �� broadcast ����
	void* baseAddress = ::malloc(sizeof(PACKET_HEADER) + sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE));
	::memcpy(baseAddress, &packetHeader, sizeof(PACKET_HEADER));
	::memcpy(static_cast<char*>(baseAddress) + sizeof(PACKET_HEADER), &broadcastPacket, sizeof(SC_ROOM_OPEN_NOTIFY_MULTIPLE));
	SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(baseAddress), true);
	::free(baseAddress);
}

void ClientPacketHandler::ProcessRoomEnter(shared_ptr<ChatSession> session, char* packetData, int size)
{
	// �ű� ������ Ŭ���̾�Ʈ���� �� ���� ���� ���θ� ������ ��Ŷ ����
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
		//  �ű� ������ Ŭ���̾�Ʈ���� RoomUserList ����
		PACKET_HEADER packetHeader;
		packetHeader.packetCount = enteredRoom->GetCurrentUserCount() - 1; // �ڱ� �ڽ��� ���ش�.
		packetHeader.size += sizeof(SC_USER_LIST_NOTIFY_MULTIPLE) * packetHeader.packetCount;
		packetHeader.id = PacketID::ROOM_USER_LIST_NOTIFY;

		void* basePacketAddress = ::malloc(packetHeader.size);
		::memcpy((PACKET_HEADER*)basePacketAddress, &packetHeader, PACKET_HEADER_SIZE);

		int i = 0;
		unique_ptr<SC_USER_LIST_NOTIFY_MULTIPLE[]> userIDs = make_unique<SC_USER_LIST_NOTIFY_MULTIPLE[]>(packetHeader.packetCount);
		for (auto& user : enteredRoom->GetUserList())
		{
			if (user->GetSock() != session->GetSock())	// �ڱ� �ڽ��� ����Ʈ���� ����
				::memcpy(userIDs[i++].userID, user->GetUserID(), strlen(user->GetUserID()));
		}
		::memcpy(static_cast<char*>(basePacketAddress) + PACKET_HEADER_SIZE, userIDs.get(), sizeof(SC_USER_LIST_NOTIFY_MULTIPLE) * packetHeader.packetCount);
		SendProcessedPacket(session, reinterpret_cast<PACKET_HEADER*>(basePacketAddress));


		// ���� �濡 �������ִ� Ŭ���̾�Ʈ�鿡�� �ű� ������ UserID ����
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
		if (enteredRoom->GetCurrentUserCount() == 0)	// Room�� ������ �������� �ʴ´ٸ� Room Close
		{
			SC_ROOM_CLOSE closePacket;
			closePacket.roomNumber = enteredRoom->GetRoomNumber();
			GRoomManager->CloseRoom(enteredRoom->GetRoomNumber());
			SendProcessedPacket(shared_ptr<ChatSession>(), &closePacket, true); // �ڱ� �ڽ��� ������ ��� �������� ��Ŷ ����
		}
		else
		{
			SC_ROOM_LEAVE_USER_NOTIFY multicastPacket;
			memcpy(multicastPacket.userID, session->GetUserID(), strlen(session->GetUserID()));
			SendUserNotifyPacket(session, &multicastPacket);
		}
	}
}
