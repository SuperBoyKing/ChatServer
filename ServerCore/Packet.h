#pragma once

#pragma pack(1)
enum class PacketID : unsigned __int16
{
	LOGIN_REQUEST,
	LOGIN_RESPONSE,

	CHAT_REQUEST,
	CHAT_RESPONSE,

	ROOM_ENTER_REQUEST,
	ROOM_ENTER_RESPONSE,

	ROOM_LEAVE_REQUEST,
	ROOM_LEAVE_RESPONSE,
};

struct PACKET_HEADER
{
	unsigned int	size;
	PacketID		id;
};

struct CS_LOGIN_REQUEST : public PACKET_HEADER
{
	char userID[32 + 1] = {};
	char userPW[32 + 1] = {};

	CS_LOGIN_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::LOGIN_REQUEST;
	}
};

struct SC_LOGIN_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_LOGIN_RESPONSE()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::LOGIN_RESPONSE;
	}
};

struct CS_CHAT_REQUEST : public PACKET_HEADER
{
	char message[256 + 1] = {};

	CS_CHAT_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::CHAT_REQUEST;
	}
};

struct SC_CHAT_REQUEST : public PACKET_HEADER
{
	char message[256 + 1] = {};

	SC_CHAT_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::CHAT_REQUEST;
	}
};

struct SC_CHAT_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_CHAT_RESPONSE()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::CHAT_RESPONSE;
	}
};

struct CS_ROOM_ENTER_REQUEST : public PACKET_HEADER
{
	int roomNumber = 0;
	
	CS_ROOM_ENTER_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::CHAT_REQUEST;
	}
};

struct SC_ROOM_ENTER_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_ROOM_ENTER_RESPONSE()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::ROOM_ENTER_RESPONSE;
	}
};

struct CS_ROOM_LEAVE_REQUEST : public PACKET_HEADER
{
	int roomNumber = 0;

	CS_ROOM_LEAVE_REQUEST()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::ROOM_LEAVE_REQUEST;
	}
};

struct SC_ROOM_LEAVE_RESPONSE : public PACKET_HEADER
{
	bool result = false;

	SC_ROOM_LEAVE_RESPONSE()
	{
		size = PACKET_HEADER_SIZE;
		id = PacketID::ROOM_LEAVE_RESPONSE;
	}
};

#pragma pack()

