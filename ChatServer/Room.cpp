#include "pch.h"
#include "Room.h"
#include "RoomManager.h"

class RoomManager;

Room::Room(unsigned int roomNumber)
	:	m_title{}
	,	m_roomNumber(roomNumber)
	,	m_maxUserCount(0)
	,	m_currentUserCount(0)
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
	if (m_currentUserCount.fetch_add(1) >= m_maxUserCount)
		return false;

	{
		lock_guard<recursive_mutex> lock(m_mutex);
		m_userList.push_back(userSession); 
	}

	return true;
}

bool Room::Leave(shared_ptr<ChatSession> userSession)
{
	if (m_currentUserCount.fetch_sub(1) <= 0)
		return false;

	auto itr = find(m_userList.begin(), m_userList.end(), userSession);

	{
		lock_guard<recursive_mutex> lock(m_mutex);
		if (itr != m_userList.end())
		{
			m_userList.erase(itr);
			return true;
		}
	}

	return false;
}

void Room::reset()
{
	memset(m_title, 0, 33);
	m_roomNumber = 0;
	m_maxUserCount = 0;
	m_currentUserCount = 0;
	m_userList.clear();
}
