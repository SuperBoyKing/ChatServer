#pragma once
class Room
{
public:
	Room() = delete;
	Room(unsigned int roomNumber);
	~Room() = default;

	void			Init(char* roomTitle, const size_t titleSize, const int maxUserCount, const int roomNumber);

	void			Enter(shared_ptr<ChatSession> userSession);

	void			Leave(shared_ptr<ChatSession> userSession);

	void			CloseRoom();

	inline int		GetRoomNumber() const	{ return m_roomNumber; }

	inline char*	GetTitle()				{ return &m_title[0];	}

	inline int		GetMaxUserCount() const	{ return m_maxUserCount; }

private:
	unsigned int		m_roomNumber;
	char				m_title[50 + 1];
	int					m_maxUserCount;
	int					m_currentUserCount;
	recursive_mutex		m_mutex;

	list<shared_ptr<ChatSession>> m_userList;
};

