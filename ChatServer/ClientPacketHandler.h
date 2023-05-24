#pragma once
class ClientPacketHandler
{
public:
	ClientPacketHandler();
	~ClientPacketHandler();

	void HandlePacket(shared_ptr<ChatSession> session, char* packet);	// 추후 수정

	// 패킷 처리 함수 //
	void ProcessConnect(shared_ptr<ChatSession> session, char* packetData, int size);
	void ProcessLogin(shared_ptr<ChatSession> session, char* packetData, int size);
	void ProcessChat(shared_ptr<ChatSession> session, char* packetData, int size);
	void ProcessRoomOpen(shared_ptr<ChatSession> session, char* packetData, int size);
	void ProcessRoomEnter(shared_ptr<ChatSession> session, char* packetData, int size);
	void ProcessRoomLeave(shared_ptr<ChatSession> session, char* packetData, int size);

	template <typename PacketType>
	void SendProcessedPacket(shared_ptr<ChatSession> session, PacketType* packet, bool isBroadcast = false)
	{
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(packet->size);
		sendBuffer->CopyData(reinterpret_cast<void*>(packet), packet->size);
		if (isBroadcast == false)
		{
			GClientSessionManager->SendToSession(session, sendBuffer);
		}
		else
		{
			GClientSessionManager->Broadcast(session, sendBuffer);
		}
	}

	using ProcessPacket = function<void(ClientPacketHandler&, shared_ptr<ChatSession>, char*, int)>;

private:
	unordered_map<PacketID, ProcessPacket> m_uMapProcessPacket;
};

extern unique_ptr<ClientPacketHandler> GClientPacketHandler;
