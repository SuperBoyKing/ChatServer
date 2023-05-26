using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace WinFormClient
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Room
    {
        public Room(int number, string title, int maxUserCount, int currentUserCount)
        {
            this.number = number;
            this.title = title;
            this.maxUserCount = maxUserCount;
            this.currentUserCount = currentUserCount;
        }

        public int number;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 51)]
        public string title;
        public int maxUserCount;
        public int currentUserCount;
    }

    // Room List 관리를 위한 클라이언트 UI 내부 자료구조
    class RoomManager
    {
       public Dictionary<string, Room> roomDictionary = new Dictionary<string, Room>();
    }
}
