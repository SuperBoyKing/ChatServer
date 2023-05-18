#pragma once
class ClientPacketHandler
{
public:
	ClientPacketHandler();
	~ClientPacketHandler();

	void HandlePacket(SOCKET socket, char* packet);

	void ProcessChat(SOCKET socket, char* packetData, int size);
	void ProcessLogin(SOCKET socket, char* packetData, int size);
	void ProcessRoomOpen(SOCKET socket, char* packetData, int size);
	void ProcessRoomEnter(SOCKET socket, char* packetData, int size);
	void ProcessRoomLeave(SOCKET socket, char* packetData, int size);

	using ProcessPacket = function<void(ClientPacketHandler&, SOCKET, char*, int)>;

private:
	unordered_map<PacketID, ProcessPacket> m_uMapProcessPacket;
};

extern unique_ptr<ClientPacketHandler> GClientPacketHandler;
