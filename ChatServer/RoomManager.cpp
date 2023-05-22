#include "pch.h"
#include "Room.h"
#include "RoomManager.h"

unique_ptr<RoomManager> GRoomManager = make_unique<RoomManager>();

RoomManager::RoomManager()
	: m_maxRoomCount(MAX_ROOM_COUNT)
	, m_roomPoolIndex(0)
{
	m_roomPool.reserve(m_maxRoomCount);
	Init();
}

RoomManager::~RoomManager()
{
	m_roomPool.clear();
}

void RoomManager::Init()
{
	for (int i = 1; i <= m_maxRoomCount; ++i)
	{
		m_roomPool.push_back(make_shared<Room>(0));
	}
}

bool RoomManager::OpenRoom(const char* roomTitle, const size_t titleSize, const int maxUserCount, OUT int* roomNumber)
{
	bool bResult = false;
	shared_ptr<Room> room = nullptr;

	mutex m;
	lock_guard<mutex> lock(m); // ���Ŀ� lock-free�� ����
	
	int poolIndexCycle = m_roomPoolIndex - 1;

	if (poolIndexCycle <= 0)
		poolIndexCycle = MAX_ROOM_COUNT;

	while (poolIndexCycle != m_roomPoolIndex)
	{
		room = m_roomPool[m_roomPoolIndex++];
		if (room->GetRoomNumber() == 0)
		{
			room->Init(roomTitle, titleSize, maxUserCount, m_roomPoolIndex);
			*roomNumber = m_roomPoolIndex;
			bResult = true;
			m_currentOpenRoomCount.fetch_add(1);
			break;
		}
	}

	if (m_roomPoolIndex >= MAX_ROOM_COUNT)
		m_roomPoolIndex = 0;

	return bResult;
}
