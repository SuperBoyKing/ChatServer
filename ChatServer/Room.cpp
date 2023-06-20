#include "pch.h"
#include "Room.h"
#include "RoomManager.h"

class RoomManager;

Room::Room(unsigned int roomNumber)
	:	m_title{}
	,	m_roomNumber(roomNumber)
	,	m_maxUserCount(0)
{
}

void Room::Init(const char* roomTitle, const size_t titleSize, const int maxUserCount, const int roomNumber)
{
	::memcpy(m_title, roomTitle, titleSize);
	m_roomNumber = roomNumber;
	m_maxUserCount = maxUserCount;
}

bool Room::Enter(shared_ptr<ChatSession> userSession)
{
	lock_guard<mutex> lock(m_mutex);
	m_userList.push_back(userSession);

	return true;
}

bool Room::Leave(shared_ptr<ChatSession> userSession)
{
	lock_guard<mutex> lock(m_mutex);
	auto itr = find(m_userList.begin(), m_userList.end(), userSession);
	if (itr != m_userList.end())
	{
		m_userList.erase(itr);
		return true;
	}

	return false;
}

void Room::MakeUserListPacket(OUT void* basePacketHeaderAddress, IN int packetCount)
{
	LONGLONG i = 0;

	{
		lock_guard<mutex> lock(m_mutex);
		for (auto& user : m_userList)
		{
			::memcpy(static_cast<char*>(basePacketHeaderAddress) + PACKET_HEADER_SIZE + ((MAX_USER_ID_LENGTH + 1) * i),
				user->GetUserID(), sizeof(char) * MAX_USER_ID_LENGTH + 1);
			++i;

			if (i == packetCount)
				break;
		}
	}
}

void Room::reset()
{
	memset(m_title, 0, MAX_ROOM_TITLE_LENGTH + 1);
	m_roomNumber = 0;
	m_maxUserCount = 0;
	m_userList.clear();
}
