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
        CHAT_NOTIFY,

        ROOM_OPEN_REQUEST,
        ROOM_OPEN_RESPONSE,

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
    public struct SC_CHAT_NOTIFY
    {
        public PACKET_HEADER header;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 257)]
        public string message;
    }

    public struct CS_ROOM_OPEN_REQUEST
    {
        public PACKET_HEADER header;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 129)]
        public string roomTitle;
        public int userCount;
    }

    public struct SC_ROOM_OPEN_RESPONSE
    {
        public PACKET_HEADER header;
        public int roomNumber;
        public bool result;
    }

    public struct CS_ROOM_ENTER_REQUEST
    {
        public PACKET_HEADER header;
        public int roomNumber;
    }

    public struct SC_ROOM_ENTER_RESPONSE
    {
        public PACKET_HEADER header;
        public bool result;
    }
}
