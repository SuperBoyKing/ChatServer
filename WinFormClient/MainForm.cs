using System;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Drawing;

namespace WinFormClient
{
    enum ChatDrawingType
    {
        NONE,
        RESPONSE_CHAT,
        NOTIFY_ID,
        NOTIFY_CHAT,
        INFO,
    }
    public partial class MainForm : Form
    {

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ChatClientStart([MarshalAs(UnmanagedType.LPWStr)] String ip, [MarshalAs(UnmanagedType.I2)] short port);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendConnectPacket();

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendLoginPacket([MarshalAs(UnmanagedType.LPStr)] string id, int idSize, [MarshalAs(UnmanagedType.LPStr)] string pwd, int pwdSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendChatPacket([MarshalAs(UnmanagedType.LPStr)] string str, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomOpenPacket([MarshalAs(UnmanagedType.LPStr)] string str, int titleSize, int userCount);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomEnterPacket(int number);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomLeavePacket(int number);

        ChatDrawingType[] chatDrawIndexFlags = new ChatDrawingType[512];

        // 로컬 ChatApplication 정보
        string userID = null;
        string roomTitle = null;
        int roomMaxUserCount = 0;
        RoomManager roomManager = new RoomManager();

        // flag
        bool IsActivatedLogin = false;
        bool IsActivatedConnect = false;

        public MainForm()
        {
            InitializeComponent();  
        }

        public void mainForm_Load(object sender, EventArgs e)
        {
            InitProcessPacket();

            listView_room.View = View.Details;
            listView_room.FullRowSelect = true;
            listView_room.Columns.Add("ID", 0, HorizontalAlignment.Left);
            listView_room.Columns.Add("Title", 170, HorizontalAlignment.Left);
            listView_room.Columns.Add("Users", 40, HorizontalAlignment.Left);
            listView_room.Columns.Add("Max", 40, HorizontalAlignment.Left);

            System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();
            timer.Interval = 100;
            timer.Tick += new EventHandler(BackGroundRecvProcess);
            timer.Start();
        }

        private void mainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            roomManager.roomDictionary.Clear();
        }

        private void Button_isConnect_Click(object sender, EventArgs e)
        {
            if(!IsActivatedConnect)
            {
                ChatClientStart(textBox_IP.Text, Int16.Parse(textBox_port.Text));
                SendConnectPacket();

                // Connect Disable
                textBox_IP.Enabled = false;
                textBox_port.Enabled = false;

                // Login Setup
                textBox_ID.Enabled = true;
                textBox_password.Enabled = true;
                button_login.Enabled = true;
                
                IsActivatedConnect = true;
            }
            else
            {
                // Connect Setup
                textBox_IP.Enabled = true;
                textBox_port.Enabled = true;

                // Login Disable
                textBox_ID.Enabled = false;
                textBox_password.Enabled = false;
                button_login.Enabled = false;

                IsActivatedConnect = false;
            }
        }

        private void Button_login_Click(object sender, EventArgs e)
        {
            if (!IsActivatedLogin)
            {
                SendLoginPacket(textBox_ID.Text, textBox_ID.Text.Length, textBox_password.Text, textBox_password.Text.Length);
            }
            else
            {
                userID = null;
                button_login.Text = "Login";
                textBox_ID.Text = "";
                textBox_ID.Enabled = true;
                textBox_password.Enabled = true;
                textBox_password.Text = "";
                IsActivatedLogin = false;
            }
        }

        private void Button_chat_Click(object sender, EventArgs e)
        {
            string chatMsg = textBox_chat.Text;
            SendChatPacket(chatMsg, chatMsg.Length);
            button_chat.Enabled = false;
        }

        private void ListBox_user_DrawEvent(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();

            if (e.Index == -1)
                return;

            if (e.Index == 0)
                e.Graphics.DrawString(listBox_user.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Bold), Brushes.Black, e.Bounds);
            else
                e.Graphics.DrawString(listBox_user.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Regular), Brushes.Black, e.Bounds);
        }

        private void ListBox_chat_DrawEvent(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();

            if (e.Index == -1)
                return;
            
            switch (chatDrawIndexFlags[e.Index])
            {
                case ChatDrawingType.RESPONSE_CHAT:
                    StringFormat strFormat = new StringFormat();
                    strFormat.Alignment = StringAlignment.Far;
                    e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Bold), Brushes.Black, e.Bounds, strFormat);
                    break;

                case ChatDrawingType.NOTIFY_ID:
                    e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Bold), Brushes.Black, e.Bounds);
                    break;

                case ChatDrawingType.NOTIFY_CHAT:
                    e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Regular), Brushes.Black, e.Bounds);
                    break;

                case ChatDrawingType.INFO:
                    StringFormat strInfoFormat = new StringFormat();
                    strInfoFormat.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Regular), Brushes.Black, e.Bounds, strInfoFormat);
                    //D3D3D3
                    break;
            }
        }

        private void button_RoomCreate_Click(object sender, EventArgs e)
        {
            RoomCreator roomCreator = new RoomCreator();
            var result = roomCreator.ShowDialog();

            if (result == DialogResult.OK)
            {
                roomTitle = roomCreator.returnTitle;
                roomMaxUserCount = roomCreator.returnUserCount;
                SendRoomOpenPacket(roomTitle, roomTitle.Length, roomMaxUserCount);
            }
        }

        private void button_RoomEnter_Click(object sender, EventArgs e)
        {
            Room room;
            int key = Int32.Parse(listView_room.SelectedItems[0].SubItems[0].Text);

            if (roomManager.roomDictionary.TryGetValue(key, out room))
            {
                SendRoomEnterPacket(key);
                button_RoomEnter.Enabled = false;
            }
            else
            {
                MessageBox.Show("Does not exist room.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button_RoomLeave_Click(object sender, EventArgs e)
        {
            Room room;
            int index = listView_room.SelectedItems.Count - 1;
            if (index == -1)
                return;

            int key = Int32.Parse(listView_room.SelectedItems[index].SubItems[0].Text);
            if (roomManager.roomDictionary.TryGetValue(key, out room))
            {
                SendRoomLeavePacket(key);
            }
            else
            {
                MessageBox.Show("Does not exist room.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        void BackGroundRecvProcess(object sender, EventArgs e)
        {
            ProcessPacket();
        }

        void AddRoomListUI(Room room)
        {
            roomManager.roomDictionary.Add(room.number, room);
            ListViewItem newItem = new ListViewItem(new string[] { room.number.ToString(), room.title, room.currentUserCount.ToString(), room.maxUserCount.ToString() });
            listView_room.Items.Add(newItem);
            listView_room.Items[0].Selected = true;
        }

        void RemoveRoomListUI(int key)
        {
            roomManager.roomDictionary.Remove(key);
        }

        void EnableRoomUI()
        {
            // Chat UI 활성화
            button_chat.Enabled = true;
            textBox_chat.Enabled = true;
            listBox_chat.Enabled = true;
            listBox_user.Enabled = true;

            // Leave 버튼 활성화
            button_RoomLeave.Enabled = true;

            // title 입력
            textBox_roomTitle.Text = listView_room.SelectedItems[0].SubItems[1].Text;
            UserCountIncreaseUI();
        }

        void DisableRoomUI()
        {
            listBox_chat.Items.Clear();
            listBox_user.Items.Clear();
            textBox_roomTitle.Clear();
            textBox_userCount.Clear();

            Room room;
            int key = Int32.Parse(listView_room.SelectedItems[0].SubItems[0].Text);
            if (roomManager.roomDictionary.TryGetValue(key, out room))
            {
                if (room.currentUserCount == 1) // 만약 남은 유저가 자기 자신밖에 없다면 List에서 room 제거
                    RemoveRoomListUI(key);
            }

            button_RoomEnter.Enabled = true;
        }

        void AddUserListUI(string userID)
        {
            listBox_user.Items.Add(userID);
        }

        void RemoveUserListUI(string userID)
        {
            if (listBox_user.FindString(userID) != ListBox.NoMatches)
            {
                listBox_user.Items.Remove(userID);
            }
        }

        void UserCountIncreaseUI()
        {
            Room room;
            int index = listView_room.SelectedItems.Count - 1;
            if (index == -1)
                return;

            int key = Int32.Parse(listView_room.SelectedItems[index].SubItems[0].Text);

            if(roomManager.roomDictionary.TryGetValue(key, out room))
            {
                room.currentUserCount += 1;
                roomManager.roomDictionary[key] = room;
                textBox_userCount.Text = room.currentUserCount.ToString();
            }
        }

        void SetLoginStateUI()
        {
            button_login.Text = "Logout";
            userID = textBox_ID.Text;
            IsActivatedLogin = true;

            textBox_ID.Enabled = false;
            textBox_password.Enabled = false;
            button_RoomCreate.Enabled = true;
            button_RoomEnter.Enabled = true;
        }

        private void listView_room_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
        {
            e.NewWidth = listView_room.Columns[e.ColumnIndex].Width;
            e.Cancel = true;
        }
    }
}
