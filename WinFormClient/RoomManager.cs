using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace WinFormClient
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Room
    {
        public Room(int number, string title, int userCount)
        {
            this.number = number;
            this.title = title;
            this.userCount = userCount;
        }

        public int number;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 51)]
        public string title;
        public int userCount;
    }

    class RoomManager
    {
       public Dictionary<string, Room> roomDictionary = new Dictionary<string, Room>();
    }
}
