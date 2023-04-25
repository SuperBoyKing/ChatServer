#pragma once
#include "pch.h"
#include "IOCPBinder.h"

IOCPBinder::IOCPBinder(IoOperation ioOperaiton) : m_ioOperation(ioOperaiton)
{
	Init();
}

void IOCPBinder::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
