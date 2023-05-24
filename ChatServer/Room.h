#pragma once
class Room
{
public:
	Room() = delete;
	Room(unsigned int roomNumber);
	~Room() = default;

	void			Init(const char* roomTitle, const size_t titleSize, const int maxUserCount, const int roomNumber);

	bool			Enter(shared_ptr<ChatSession> userSession);

	bool			Leave(shared_ptr<ChatSession> userSession);

	void			CloseRoom();

	inline int			GetRoomNumber() const			{ return m_roomNumber; }

	inline const char*	GetTitle() const				{ return &m_title[0];	}

	inline int			GetMaxUserCount() const			{ return m_maxUserCount; }

	inline int			GetCurrentUserCount() const		{ return m_currentUserCount; }

	inline list<shared_ptr<ChatSession>>& GetUserList() { return m_userList; }

private:
	int					m_roomNumber;
	char				m_title[50 + 1];
	int					m_maxUserCount;
	atomic<int>			m_currentUserCount;
	recursive_mutex		m_mutex;

	list<shared_ptr<ChatSession>> m_userList;
};

