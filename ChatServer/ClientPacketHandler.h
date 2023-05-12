#pragma once
class ClientPacketHandler : public PacketHandler
{
public:
	ClientPacketHandler();
	~ClientPacketHandler();

	void ProcessChat(SOCKET socket, char* packetData, int size) override;
	void ProcessLogin(SOCKET socket, char* packetData, int size) override;
	void ProcessRoomEnter(SOCKET socket, char* packetData, int size) override;
	void ProcessRoomLeave(SOCKET socket, char* packetData, int size) override;
};

extern unique_ptr<ClientPacketHandler> GClientPacketHandler;
