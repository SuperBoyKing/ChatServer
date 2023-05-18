#pragma once
class RoomManager
{
public:
	RoomManager();
	~RoomManager();
	
	void	Init();

	bool	OpenRoom(char* roomTitle, int titleSize, int maxUserCount);

private:
	int							m_maxRoomCount;
	atomic<int>					m_roomPoolIndex;
	vector<shared_ptr<Room>>	m_roomPool;
};

extern unique_ptr<RoomManager> GRoomManager;
