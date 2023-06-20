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

	template <typename PacketType>
	void SendUserNotifyPacket(shared_ptr<ChatSession> userSession, PacketType* packet)
	{
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(packet->size);
		sendBuffer->CopyData(reinterpret_cast<void*>(packet), packet->size);
		
		lock_guard<mutex> lock(m_mutex);
		for (auto user : m_userList)
		{
			if (userSession.get() != user.get())
				GClientSessionManager->SendToSession(sendBuffer, user);
		}
	}

	void MakeUserListPacket(OUT void* basePacketHeaderAddress, IN int packetCount);

	void				reset();

	inline int			GetRoomNumber() const			{ return m_roomNumber; }

	inline const char*	GetTitle() const				{ return &m_title[0];	}

	inline int			GetMaxUserCount() const			{ return m_maxUserCount; }

	inline long long	GetCurrentUserCount() const		{ return m_userList.size(); }

private:
	int					m_roomNumber;
	char				m_title[MAX_ROOM_TITLE_LENGTH + 1];
	int					m_maxUserCount;
	mutex				m_mutex;

	list<shared_ptr<ChatSession>> m_userList;
};

