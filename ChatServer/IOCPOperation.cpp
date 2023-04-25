#pragma once
#include "pch.h"
#include "IOCPOperation.h"

IOCPOperation::IOCPOperation(IoType ioOperaiton) : m_ioType(ioOperaiton)
{
	Init();
}

void IOCPOperation::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
