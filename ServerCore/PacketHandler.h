#pragma once

class PacketHandler
{
public:
	PacketHandler() = default;
	~PacketHandler() = default;

	void HandlePacket(SOCKET socket, char* packet, int len);

	virtual void ProcessChat(SOCKET socket, char* packetData, int size) abstract;
	virtual void ProcessLogin(SOCKET socket, char* packetData, int size) abstract;
	virtual void ProcessRoomEnter(SOCKET socket, char* packetData, int size) abstract;
	virtual void ProcessRoomLeave(SOCKET socket, char* packetData, int size) abstract;
	
	virtual void ProcessAnnouncement(SOCKET socket, char* packetData, int size) {};

	using ProcessPacket = function<void(PacketHandler&, SOCKET, char*, int)>;

protected:
	unordered_map<PacketID, ProcessPacket> m_uMapProcessPacket;
};

