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
	char userID[32 + 1];
	char userPW[32 + 1];
};

struct SC_LOGIN_RESPONSE : public PACKET_HEADER
{
	bool result;
};

struct CS_CHAT_REQUEST : public PACKET_HEADER
{
	char message[100] = { 0 };
};

struct SC_CHAT_RESPONSE : public PACKET_HEADER
{
	bool result;
};

struct CS_ROOM_ENTER_REQUEST : public PACKET_HEADER
{
	int roomNumber;
};

struct SC_ROOM_ENTER_RESPONSE : public PACKET_HEADER
{
	bool result;
};

struct CS_ROOM_LEAVE_REQUEST : public PACKET_HEADER
{
	int roomNumber;
};

struct SC_ROOM_LEAVE_REQUEST : public PACKET_HEADER
{
	bool result;
};

#pragma pack()

