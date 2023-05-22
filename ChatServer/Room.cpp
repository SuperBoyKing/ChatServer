#include "pch.h"
#include "Room.h"

Room::Room(unsigned int roomNumber)
	:	m_title{}
	,	m_roomNumber(roomNumber)
	,	m_maxUserCount(0)
	,	m_currentUserCount(0)
{
}

void Room::Init(char* roomTitle, const size_t titleSize, const int maxUserCount, const int roomNumber)
{
	::memcpy(m_title, roomTitle, titleSize);
	m_roomNumber = roomNumber;
	m_maxUserCount = maxUserCount;
}

void Room::Enter(shared_ptr<ChatSession> userSession)
{
	lock_guard<recursive_mutex> lock(m_mutex);
	m_userList.push_back(userSession);
}

void Room::Leave(shared_ptr<ChatSession> userSession)
{
	auto itr = find(m_userList.begin(), m_userList.end(), userSession);

	lock_guard<recursive_mutex> lock(m_mutex);
	if (itr != m_userList.end())
	{
		m_userList.erase(itr);

		if (m_userList.empty())
			CloseRoom();
	}
}

void Room::CloseRoom()
{
}
