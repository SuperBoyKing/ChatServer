#pragma once

#pragma pack(1)
enum class PacketID : unsigned __int16
{
	NONE,
	CONNECT_REQUEST,
	CONNECT_RESPONSE,

	LOGIN_REQUEST,
	LOGIN_RESPONSE,

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
	ROOM_LEAVE_NOTIFY,
};

struct ROOM_INFO
{
	int		number = 0;
	char	title[50+1] = { 0 };
	int		userCount = 0;
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
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::CONNECT_REQUEST;
	}
};

struct SC_CONNECT_RESPONSE
{
	int		number = 0;
	char	title[50 + 1] = { 0 };
	int		userCount = 0;
};

struct CS_LOGIN_REQUEST : public PACKET_HEADER
{
	char userID[32 + 1] = {};
	char userPW[32 + 1] = {};

	CS_LOGIN_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::LOGIN_REQUEST;
	}
};

struct SC_LOGIN_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_LOGIN_RESPONSE()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::LOGIN_RESPONSE;
	}
};

struct CS_CHAT_REQUEST : public PACKET_HEADER
{
	char message[256 + 1] = {};

	CS_CHAT_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::CHAT_REQUEST;
	}
};

struct SC_CHAT_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_CHAT_RESPONSE()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::CHAT_RESPONSE;
	}
};

struct SC_CHAT_NOTIFY : public PACKET_HEADER
{
	char message[256 + 1] = {};

	SC_CHAT_NOTIFY()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::CHAT_NOTIFY;
	}
};

struct CS_ROOM_OPEN_REQUEST : public PACKET_HEADER
{
	char	roomTitle[50 + 1] = {};
	int		userCount = 0;

	CS_ROOM_OPEN_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
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
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::ROOM_OPEN_RESPONSE;
	}
};

struct SC_ROOM_OPEN_NOTIFY
{
	ROOM_INFO roomInfo;	// 추후 수정
};

struct CS_ROOM_ENTER_REQUEST : public PACKET_HEADER
{
	int		roomNumber = 0;
	
	CS_ROOM_ENTER_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::ROOM_ENTER_REQUEST;
	}
};

struct SC_ROOM_ENTER_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_ROOM_ENTER_RESPONSE()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::ROOM_ENTER_RESPONSE;
	}
};

struct SC_ROOM_USERLIST_NOTIFY
{
	char userID[32 + 1] = {};
};

struct SC_ROOM_ENTER_USER_NOTIFY : public PACKET_HEADER
{
	char userID[32 + 1] = {};

	SC_ROOM_ENTER_USER_NOTIFY()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::ROOM_ENTER_USER_NOTIFY;
	}
};

struct CS_ROOM_LEAVE_REQUEST : public PACKET_HEADER
{
	int roomNumber = 0;

	CS_ROOM_LEAVE_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::ROOM_LEAVE_REQUEST;
	}
};

struct SC_ROOM_LEAVE_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_ROOM_LEAVE_RESPONSE()
	{
		size = PACKET_HEADER_SIZE;
		packetCount = 1;
		id = PacketID::ROOM_LEAVE_RESPONSE;
	}
};

#pragma pack()

