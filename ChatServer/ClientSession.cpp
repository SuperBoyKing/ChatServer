#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"

ClientSession::ClientSession()
{
	m_uMapPacketIDToString[PacketID::CONNECT_RESPONSE] = "Connect Response";
	m_uMapPacketIDToString[PacketID::ROOM_LIST_RESPONSE] = "Room List Response";
	m_uMapPacketIDToString[PacketID::LOGIN_RESPONSE] = "Login Response";
	m_uMapPacketIDToString[PacketID::LOGOUT_RESPONSE] = "Logout Response";
	m_uMapPacketIDToString[PacketID::REGISTER_RESPONSE] = "Registered Response";
	m_uMapPacketIDToString[PacketID::CHAT_RESPONSE] = "Chat Response";
	m_uMapPacketIDToString[PacketID::CHAT_NOTIFY] = "Chat Notify";
	m_uMapPacketIDToString[PacketID::ROOM_OPEN_RESPONSE] = "Room Open Response";
	m_uMapPacketIDToString[PacketID::ROOM_OPEN_NOTIFY] = "Room Open Notify";
	m_uMapPacketIDToString[PacketID::ROOM_ENTER_RESPONSE] = "Room Enter Response";
	m_uMapPacketIDToString[PacketID::ROOM_ENTER_USER_NOTIFY] = "Room Enter User Notify";
	m_uMapPacketIDToString[PacketID::ROOM_USER_LIST_NOTIFY] = "Room User List Notify";
	m_uMapPacketIDToString[PacketID::ROOM_LEAVE_RESPONSE] = "Room Leave Response";
	m_uMapPacketIDToString[PacketID::ROOM_LEAVE_USER_NOTIFY] = "Room Leave User Notify";
	m_uMapPacketIDToString[PacketID::ROOM_CLOSE] = "Room Close";
}

ClientSession::~ClientSession()
{
	m_uMapPacketIDToString.clear();
}

void ClientSession::OnSend(BYTE* sendData)
{
	PACKET_HEADER* packetData = reinterpret_cast<PACKET_HEADER*>(sendData);
	cout << "Send Packet: "  << m_uMapPacketIDToString[packetData->id] << ", Client Socket ID : " << GetSock() << endl;
}

void ClientSession::OnRecv(char* buffer, unsigned int len)
{
	GClientPacketHandler->HandlePacket(static_pointer_cast<ChatSession>(shared_from_this()), buffer);
}

void ClientSession::OnConnect()
{
	GClientSessionManager->Add(static_pointer_cast<ChatSession>(shared_from_this()));
}

void ClientSession::OnDisconnect()
{
	shared_ptr<ChatSession> session = static_pointer_cast<ChatSession>(shared_from_this());
	if (session->GetSessionState() == SessionState::ROOM) // 클라이언트의 비정상 종료
	{
		CS_ROOM_LEAVE_REQUEST roomPacketData;
		roomPacketData.roomNumber = session->GetRoomNumber();
		GClientPacketHandler->ProcessRoomLeave(session, reinterpret_cast<char*>(&roomPacketData), 0);
	}
	GClientSessionManager->Remove(session);
}