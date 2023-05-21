using System;
using System.Collections.Generic;
using System.Text;

namespace WinFormClient
{
    class Room
    {
        public Room(int number, string title, int userCount)
        {
            this.number = number;
            this.title = title;
            this.userCount = userCount;
        }

        public int number { get; set; }
        public string title { get; set; }
        public int userCount { get; set; }
    }

    class RoomManager
    {
       public Dictionary<string, Room> roomDictionary = new Dictionary<string, Room>();
    }
}
