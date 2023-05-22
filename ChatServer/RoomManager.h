#pragma once
class Room; 

class RoomManager
{
public:
	RoomManager();
	~RoomManager();
	
	void	Init();

	bool	OpenRoom(const char* roomTitle, const size_t titleSize, const int maxUserCount, OUT int* roomNumber);

	inline  vector<shared_ptr<Room>>& GetRoomPool() { return m_roomPool; }

	inline  int	GetOpenRoomCount() const { return m_currentOpenRoomCount; }

	shared_ptr<Room> SearchRoom();

private:
	int							m_maxRoomCount;
	atomic<int>					m_currentOpenRoomCount;
	atomic<int>					m_roomPoolIndex;
	vector<shared_ptr<Room>>	m_roomPool;
};

extern unique_ptr<RoomManager> GRoomManager;
