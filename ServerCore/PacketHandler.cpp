#include "pch.h"
#include "PacketHandler.h"

void PacketHandler::HandlePacket(SOCKET socket, char* packet)
{
	PACKET_HEADER stPacketHeader = {0,};
	::memcpy((char*)&stPacketHeader, packet, PACKET_HEADER_SIZE);

	auto itr = m_uMapProcessPacket.find(stPacketHeader.id);
	if (itr != m_uMapProcessPacket.end())
	{
		itr->second(*this, socket, packet + PACKET_HEADER_SIZE, stPacketHeader.size - PACKET_HEADER_SIZE);	// 해당 패킷 ID 함수 호출
	}
}