#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"

void ClientSession::OnSend(unsigned int len)
{
	cout << "Server Send Complete : " << len << endl;
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