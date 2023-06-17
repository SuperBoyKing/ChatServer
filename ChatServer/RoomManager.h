#pragma once
class Room; 

class RoomManager
{
public:
	RoomManager();
	~RoomManager();
	
	void	Init();

	bool	OpenRoom(const char* roomTitle, const size_t titleSize, const int maxUserCount, OUT int* roomNumber);

	bool	CloseRoom(const int roomNumber);

	inline  vector<shared_ptr<Room>>& GetRoomObjectPool() { return m_roomObjectPool; }

	inline  int	GetOpenRoomCount() const { return m_currentOpenRoomCount; }

	inline shared_ptr<Room> SearchRoom(int roomNumber)
	{
		if (roomNumber - 1 < 0)
			return shared_ptr<Room>();

		return m_roomObjectPool[(int)(roomNumber - 1)];
	}

private:
	int							m_maxRoomCount;
	atomic<int>					m_currentOpenRoomCount;
	atomic<int>					m_roomPoolIndex;
	vector<shared_ptr<Room>>	m_roomObjectPool;
	mutex m_mutex;
};

extern unique_ptr<RoomManager> GRoomManager;
