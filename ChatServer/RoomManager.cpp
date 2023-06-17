#include "pch.h"
#include "Room.h"
#include "RoomManager.h"

unique_ptr<RoomManager> GRoomManager = make_unique<RoomManager>();

RoomManager::RoomManager()
	: m_maxRoomCount(MAX_ROOM_COUNT)
	, m_currentOpenRoomCount(0)
	, m_roomPoolIndex(0)
{
	m_roomObjectPool.reserve(m_maxRoomCount);
	Init();
}

RoomManager::~RoomManager()
{
	m_roomObjectPool.clear();
}

void RoomManager::Init()
{
	for (int i = 1; i <= m_maxRoomCount; ++i)
	{
		m_roomObjectPool.push_back(make_shared<Room>(0));
	}
}

bool RoomManager::OpenRoom(const char* roomTitle, const size_t titleSize, const int maxUserCount, OUT int* roomNumber)
{
	bool bResult = false;
	shared_ptr<Room> room = nullptr;

	lock_guard<mutex> lock(m_mutex);
	
	int poolIndexCycle = m_roomPoolIndex - 1;

	if (poolIndexCycle <= 0)
		poolIndexCycle = MAX_ROOM_COUNT;

	while (poolIndexCycle != m_roomPoolIndex)
	{
		room = m_roomObjectPool[m_roomPoolIndex++];
		if (room->GetRoomNumber() == 0)
		{
			room->Init(roomTitle, titleSize, maxUserCount, m_roomPoolIndex);
			*roomNumber = m_roomPoolIndex;
			m_currentOpenRoomCount.fetch_add(1);
			bResult = true;
			break;
		}
	}

	if (m_roomPoolIndex >= MAX_ROOM_COUNT)
		m_roomPoolIndex = 0;

	return bResult;
}

bool RoomManager::CloseRoom(const int roomNumber)
{
	if (roomNumber - 1 < 0)
		return false;

	m_roomObjectPool[(int)(roomNumber - 1)]->reset();
	m_currentOpenRoomCount.fetch_sub(1);
	return true;
}



