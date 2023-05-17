using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace WinFormClient
{
    public enum PacketID : ushort
    {
        LOGIN_REQUEST,
        LOGIN_RESPONSE,

        CHAT_REQUEST,
        CHAT_RESPONSE,

        ROOM_ENTER_REQUEST,
        ROOM_ENTER_RESPONSE,

        ROOM_LEAVE_REQUEST,
        ROOM_LEAVE_RESPONSE,
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PACKET_HEADER
    {
        public int size;
        public PacketID id;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct SC_CHAT_REQUEST
    {
        public PACKET_HEADER header;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 257)]
        public string message;
    }
}
