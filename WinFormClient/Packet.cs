using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace WinFormClient
{
    public enum PacketID : ushort
    {
        NONE,
        CONNECT_REQUEST,
        CONNECT_RESPONSE,

        LOGIN_REQUEST,
        LOGIN_RESPONSE,

        CHAT_REQUEST,
        CHAT_RESPONSE,
        CHAT_NOTIFY,

        ROOM_OPEN_REQUEST,
        ROOM_OPEN_RESPONSE,
        ROOM_OPEN_NOTIFY,

        ROOM_ENTER_REQUEST,
        ROOM_ENTER_RESPONSE,
        ROOM_ENTER_USER_NOTIFY,
        ROOM_USER_LIST_NOTIFY,

        ROOM_LEAVE_REQUEST,
        ROOM_LEAVE_RESPONSE,
        ROOM_LEAVE_NOTIFY,
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PACKET_HEADER
    {
        public int size;
        public int packetCount;
        public PacketID id;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct CS_CONNECT_REQUEST
    {
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct SC_CONNECT_RESPONSE
    {
        public Room roomInfo;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct SC_CHAT_NOTIFY
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 257)]
        public string message;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct CS_ROOM_OPEN_REQUEST
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 51)]
        public string roomTitle;
        public int userCount;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct SC_ROOM_OPEN_RESPONSE
    {
        public int roomNumber;
        public bool result;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct SC_ROOM_OPEN_NOTIFY
    {
        public Room roomInfo;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct CS_ROOM_ENTER_REQUEST
    {
        public int roomNumber;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct SC_ROOM_ENTER_RESPONSE
    {
        public bool result;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct SC_ROOM_USERLIST_NOTIFY
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 33)]
        public string userID;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct SC_ROOM_ENTER_USER_NOTIFY
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 33)]
        public string userID;
    }
}
