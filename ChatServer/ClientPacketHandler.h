#pragma once
class ClientPacketHandler
{
public:
	ClientPacketHandler();
	~ClientPacketHandler();

	void HandlePacket(SOCKET socket, char* packet);

	void ProcessConnect(SOCKET socket, char* packetData, int size);
	void ProcessChat(SOCKET socket, char* packetData, int size);
	void ProcessLogin(SOCKET socket, char* packetData, int size);
	void ProcessRoomOpen(SOCKET socket, char* packetData, int size);
	void ProcessRoomEnter(SOCKET socket, char* packetData, int size);
	void ProcessRoomLeave(SOCKET socket, char* packetData, int size);

	template <typename PacketType>
	void SendProcessedPacket(SOCKET socket, PacketType& packet)
	{
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(packet.size);
		sendBuffer->CopyData(&packet, packet.size);
		GClientSessionManager->SendToSession(socket, sendBuffer);
	}

	using ProcessPacket = function<void(ClientPacketHandler&, SOCKET, char*, int)>;

private:
	unordered_map<PacketID, ProcessPacket> m_uMapProcessPacket;
};

extern unique_ptr<ClientPacketHandler> GClientPacketHandler;
