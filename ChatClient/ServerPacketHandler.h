#pragma once
class ServerPacketHandler : public PacketHandler
{
public:
	ServerPacketHandler();
	~ServerPacketHandler();

	void ProcessChat(SOCKET socket, char* packetData, int size) override;
	void ProcessAnnouncement(SOCKET socket, char* packetData, int size);
	void ProcessLogin(SOCKET socket, char* packetData, int size) override;
	void ProcessRoomEnter(SOCKET socket, char* packetData, int size) override;
	void ProcessRoomLeave(SOCKET socket, char* packetData, int size) override;
};

extern unique_ptr<ServerPacketHandler> GServerPacketHandler;