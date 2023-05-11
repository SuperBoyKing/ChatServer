#pragma once
class PacketHandler
{
public:
	PacketHandler();
	~PacketHandler();

	void Init();

	int PacketRecv(BYTE* buffer, int len);
private:

};

