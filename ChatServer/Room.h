#pragma once
class Room
{
public:
	Room() = delete;
	Room(unsigned int roomNumber);
	~Room() = default;

	void			Init(char* roomTitle, int titleSize, int maxUserCount, int roomNumber);

	void			Enter(shared_ptr<ChatSession> userSession);

	void			Leave(shared_ptr<ChatSession> userSession);

	void			CloseRoom();

	inline int		GetRoomNumber() { return m_roomNumber; }

private:
	unsigned int		m_roomNumber;
	char*				m_title;
	int					m_maxUserCount;
	int					m_currentUserCount;
	recursive_mutex		m_mutex;

	list<shared_ptr<ChatSession>> m_userList;
};

