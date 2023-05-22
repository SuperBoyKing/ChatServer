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

	//void SendConnetReqPacket(SOCKET socket, SC_CONNECT_RESPONSE* connectResPacket)
	//{
	//	shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(connectResPacket->size);
	//	sendBuffer->CopyData(reinterpret_cast<void*>(connectResPacket), connectResPacket->size);
	//	GClientSessionManager->SendToSession(socket, sendBuffer);
	//}

	template <typename PacketType>
	void SendProcessedPacket(SOCKET socket, PacketType* packet)
	{
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(packet->size);
		sendBuffer->CopyData(reinterpret_cast<void*>(packet), packet->size);
		GClientSessionManager->SendToSession(socket, sendBuffer);
	}

	template <typename PacketType>
	void SendBroadcastPacket(SOCKET socket, PacketType* packet)
	{
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(packet->size);
		sendBuffer->CopyData(reinterpret_cast<void*>(packet), packet->size);
		GClientSessionManager->Broadcast(socket, sendBuffer);
	}

	using ProcessPacket = function<void(ClientPacketHandler&, SOCKET, char*, int)>;

private:
	unordered_map<PacketID, ProcessPacket> m_uMapProcessPacket;
};

extern unique_ptr<ClientPacketHandler> GClientPacketHandler;
