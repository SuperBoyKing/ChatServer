#pragma once

#pragma pack(1)
enum class PacketID : unsigned __int8
{
	NONE,

	CONNECT_REQUEST,
	CONNECT_RESPONSE,

	DISCONNECT_REQUEST,

	ROOM_LIST_REQUEST,
	ROOM_LIST_RESPONSE,

	REGISTER_REQUEST,
	REGISTER_RESPONSE,

	LOGIN_REQUEST,
	LOGIN_RESPONSE,

	LOGOUT_REQUEST,
	LOGOUT_RESPONSE,

	CHAT_REQUEST,
	CHAT_RESPONSE,
	CHAT_NOTIFY,

	ROOM_OPEN_REQUEST,
	ROOM_OPEN_RESPONSE,
	ROOM_OPEN_NOTIFY,

	ROOM_ENTER_REQUEST,
	ROOM_ENTER_RESPONSE,
	ROOM_ENTER_USER_NOTIFY,
	ROOM_USER_LIST_NOTIFY,

	ROOM_LEAVE_REQUEST,
	ROOM_LEAVE_RESPONSE,
	ROOM_LEAVE_USER_NOTIFY,
	ROOM_CLOSE
};

struct PACKET_HEADER
{
	int				size = PACKET_HEADER_SIZE;		// header + packetData
	int				packetCount = 1;				// Packet data counts, the header included. 
	PacketID		id = PacketID::NONE;
};

struct CS_CONNECT_REQUEST : public PACKET_HEADER
{
	CS_CONNECT_REQUEST()
	{
		size = sizeof(CS_CONNECT_REQUEST);
		packetCount = 1;
		id = PacketID::CONNECT_REQUEST;
	}
};

struct CS_DISCONNECT_REQEUST : public PACKET_HEADER
{
	CS_DISCONNECT_REQEUST()
	{
		size = sizeof(CS_DISCONNECT_REQEUST);
		packetCount = 1;
		id = PacketID::DISCONNECT_REQUEST;
	}
};

struct SC_CONNECT_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_CONNECT_RESPONSE()
	{
		size = sizeof(SC_CONNECT_RESPONSE);
		packetCount = 1;
		id = PacketID::CONNECT_RESPONSE;
	}
};

struct CS_ROOM_LIST_REQUEST : public PACKET_HEADER
{
	CS_ROOM_LIST_REQUEST()
	{
		size = sizeof(CS_ROOM_LIST_REQUEST);
		packetCount = 1;
		id = PacketID::ROOM_LIST_REQUEST;
	}
};

struct SC_ROOM_LIST_MULTIPLE
{
	int		number = 0;
	char	title[MAX_ROOM_TITLE_LENGTH + 1] = { 0 };
	int		currentUserCount = 0;
	int		maxUserCount = 0;
};

struct CS_RESGISTER_REQUEST : public PACKET_HEADER
{
	char userID[MAX_USER_ID_LENGTH + 1] = {};
	char userPW[MAX_USER_PW_LENGTH + 1] = {};

	CS_RESGISTER_REQUEST()
	{
		size = sizeof(CS_RESGISTER_REQUEST);
		packetCount = 1;
		id = PacketID::REGISTER_REQUEST;
	}
};

struct SC_RESGISTER_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_RESGISTER_RESPONSE()
	{
		size = sizeof(SC_RESGISTER_RESPONSE);
		packetCount = 1;
		id = PacketID::REGISTER_RESPONSE;
	}
};

struct CS_LOGIN_REQUEST : public PACKET_HEADER
{
	char userID[MAX_USER_ID_LENGTH + 1] = {};
	char userPW[MAX_USER_PW_LENGTH + 1] = {};

	CS_LOGIN_REQUEST()
	{
		size = sizeof(CS_LOGIN_REQUEST);
		packetCount = 1;
		id = PacketID::LOGIN_REQUEST;
	}
};

struct SC_LOGIN_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_LOGIN_RESPONSE()
	{
		size = sizeof(SC_LOGIN_RESPONSE);
		packetCount = 1;
		id = PacketID::LOGIN_RESPONSE;
	}
};

struct CS_LOGOUT_REQUEST : public PACKET_HEADER
{
	char userID[MAX_USER_ID_LENGTH + 1] = {};

	CS_LOGOUT_REQUEST()
	{
		size = sizeof(CS_LOGIN_REQUEST);
		packetCount = 1;
		id = PacketID::LOGOUT_REQUEST;
	}
};

struct SC_LOGOUT_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_LOGOUT_RESPONSE()
	{
		size = sizeof(SC_LOGIN_RESPONSE);
		packetCount = 1;
		id = PacketID::LOGOUT_RESPONSE;
	}
};

struct CS_CHAT_REQUEST : public PACKET_HEADER
{
	char message[MAX_CHAT_LENGTH + 1] = {};

	CS_CHAT_REQUEST()
	{
		size = sizeof(CS_CHAT_REQUEST);
		packetCount = 1;
		id = PacketID::CHAT_REQUEST;
	}
};

struct SC_CHAT_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_CHAT_RESPONSE()
	{
		size = sizeof(SC_CHAT_RESPONSE);
		packetCount = 1;
		id = PacketID::CHAT_RESPONSE;
	}
};

struct SC_CHAT_NOTIFY : public PACKET_HEADER
{
	char userID[MAX_USER_ID_LENGTH + 1] = {};
	char message[MAX_CHAT_LENGTH + 1] = {};

	SC_CHAT_NOTIFY()
	{
		size = sizeof(SC_CHAT_NOTIFY);
		packetCount = 1;
		id = PacketID::CHAT_NOTIFY;
	}
};

struct CS_ROOM_OPEN_REQUEST : public PACKET_HEADER
{
	char	roomTitle[MAX_ROOM_TITLE_LENGTH + 1] = {};
	int		userCount = 0;

	CS_ROOM_OPEN_REQUEST()
	{
		size = sizeof(CS_ROOM_OPEN_REQUEST);
		packetCount = 1;
		id = PacketID::ROOM_OPEN_REQUEST;
	}
};

struct SC_ROOM_OPEN_RESPONSE : public PACKET_HEADER
{
	int		roomNumber = 0;
	bool	result = false;

	SC_ROOM_OPEN_RESPONSE()
	{
		size = sizeof(SC_ROOM_OPEN_RESPONSE);
		packetCount = 1;
		id = PacketID::ROOM_OPEN_RESPONSE;
	}
};

struct SC_ROOM_OPEN_NOTIFY_MULTIPLE
{
	int		number = 0;
	char	title[MAX_ROOM_TITLE_LENGTH + 1] = { 0 };
	int		currentUserCount = 0;
	int		maxUserCount = 0;
};

struct CS_ROOM_ENTER_REQUEST : public PACKET_HEADER
{
	int		roomNumber = 0;
	
	CS_ROOM_ENTER_REQUEST()
	{
		size = sizeof(CS_ROOM_ENTER_REQUEST);
		packetCount = 1;
		id = PacketID::ROOM_ENTER_REQUEST;
	}
};

struct SC_ROOM_ENTER_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_ROOM_ENTER_RESPONSE()
	{
		size = sizeof(SC_ROOM_ENTER_RESPONSE);
		packetCount = 1;
		id = PacketID::ROOM_ENTER_RESPONSE;
	}
};

struct SC_ROOM_ENTER_USER_NOTIFY : public PACKET_HEADER
{
	char userID[MAX_USER_ID_LENGTH + 1] = {};

	SC_ROOM_ENTER_USER_NOTIFY()
	{
		size = sizeof(SC_ROOM_ENTER_USER_NOTIFY);
		packetCount = 1;
		id = PacketID::ROOM_ENTER_USER_NOTIFY;
	}
};

struct SC_USER_LIST_NOTIFY_MULTIPLE
{
	char userID[MAX_USER_ID_LENGTH + 1] = {};
};

struct CS_ROOM_LEAVE_REQUEST : public PACKET_HEADER
{
	int roomNumber = 0;

	CS_ROOM_LEAVE_REQUEST()
	{
		size = sizeof(CS_ROOM_LEAVE_REQUEST);
		packetCount = 1;
		id = PacketID::ROOM_LEAVE_REQUEST;
	}
};

struct SC_ROOM_LEAVE_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_ROOM_LEAVE_RESPONSE()
	{
		size = sizeof(SC_ROOM_LEAVE_RESPONSE);
		packetCount = 1;
		id = PacketID::ROOM_LEAVE_RESPONSE;
	}
};

struct SC_ROOM_LEAVE_USER_NOTIFY : public PACKET_HEADER
{
	char userID[MAX_USER_ID_LENGTH + 1] = {};

	SC_ROOM_LEAVE_USER_NOTIFY()
	{
		size = sizeof(SC_ROOM_LEAVE_USER_NOTIFY);
		packetCount = 1;
		id = PacketID::ROOM_LEAVE_USER_NOTIFY;
	}
};

struct SC_ROOM_CLOSE : public PACKET_HEADER
{
	int roomNumber = 0;
	SC_ROOM_CLOSE()
	{
		size = sizeof(SC_ROOM_CLOSE);
		packetCount = 1;
		id = PacketID::ROOM_CLOSE;
	}
};

#pragma pack()

