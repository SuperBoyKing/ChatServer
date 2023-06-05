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
        static extern void ChatInit();

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ChatClientStart([MarshalAs(UnmanagedType.LPWStr)] String ip, [MarshalAs(UnmanagedType.I2)] short port);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void Disconnect();

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendConnectPacket();

        [DllImport("ChatClient.dll")]
        static extern void SendRegisterAccount([MarshalAs(UnmanagedType.LPStr)] string id, int idSize, [MarshalAs(UnmanagedType.LPStr)] string pwd, int pwdSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendLoginPacket([MarshalAs(UnmanagedType.LPStr)] string id, int idSize, [MarshalAs(UnmanagedType.LPStr)] string pwd, int pwdSize);

        [DllImport("ChatClient.dll")]
        static extern void SendLogoutPacket([MarshalAs(UnmanagedType.LPStr)] string id, int idSize);

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

        // ChatApplication SubForm
        RoomCreator roomCreator = new RoomCreator();
        Register registerAccount = new Register();

        // flag
        bool IsActivatedLogin = false;
        bool IsActivatedConnect = false;

        public MainForm()
        {
            InitializeComponent();
            ChatInit();
        }

        public void mainForm_Load(object sender, EventArgs e)
        {
            InitProcessPacket();

            listView_room.View = View.Details;
            listView_room.FullRowSelect = true;
            listView_room.Columns.Add("ID", 0, HorizontalAlignment.Left);
            listView_room.Columns.Add("Title", 180, HorizontalAlignment.Left);
            listView_room.Columns.Add("Max Users", 80, HorizontalAlignment.Left);

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

                // Connect Disable
                textBox_IP.Enabled = false;
                textBox_port.Enabled = false;

                // Login Setup
                textBox_ID.Enabled = true;
                textBox_password.Enabled = true;
                button_login.Enabled = true;
                button_register.Enabled = true;
                
                IsActivatedConnect = true;
                button_isConnect.Text = "Disconnect";
            }
            else
            {
                Disconnect();

                // Connect Setup
                textBox_IP.Enabled = true;
                textBox_port.Enabled = true;

                // Login Disable
                textBox_ID.Enabled = false;
                textBox_password.Enabled = false;
                button_login.Enabled = false;
                button_register.Enabled = false;

                IsActivatedConnect = false;
                button_isConnect.Text = "Connect";
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
                SendLogoutPacket(textBox_ID.Text, textBox_ID.Text.Length);
            }
        }

        private void button_register_Click(object sender, EventArgs e)
        {
            var result = registerAccount.ShowDialog();

            if (result == DialogResult.OK)
            {
                SendRegisterAccount(registerAccount.returnID, registerAccount.returnID.Length, registerAccount.returnPW, registerAccount.returnPW.Length);
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
            var result = roomCreator.ShowDialog();
            roomCreator.returnTitle = "";
            roomCreator.returnUserCount = 0;

            if (result == DialogResult.OK)
            {
                roomTitle = roomCreator.returnTitle;
                roomMaxUserCount = roomCreator.returnUserCount;
                SendRoomOpenPacket(roomTitle, roomTitle.Length, roomMaxUserCount);
            }
        }

        private void button_RoomEnter_Click(object sender, EventArgs e)
        {
            int index = listView_room.SelectedItems.Count - 1;
            if (index == -1)
                return;

            int key = Int32.Parse(listView_room.SelectedItems[index].SubItems[0].Text);

            if (roomManager.roomDictionary.ContainsKey(key))
            {
                SendRoomEnterPacket(key);
            }
            else
            {
                MessageBox.Show("Does not exist room.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button_RoomLeave_Click(object sender, EventArgs e)
        {
            int index = listView_room.SelectedItems.Count - 1;
            if (index == -1)
                return;

            int key = Int32.Parse(listView_room.SelectedItems[index].SubItems[0].Text);

            if (roomManager.roomDictionary.ContainsKey(key))
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
            if (listView_room.Items.Count != 0)
                button_RoomEnter.Enabled = true;
            else
                button_RoomEnter.Enabled = false;

            ProcessPacket();
        }

        void AddRoomListUI(Room room)
        {
            roomManager.roomDictionary.Add(room.number, room);
            ListViewItem newItem = new ListViewItem(new string[] { room.number.ToString(), room.title, room.maxUserCount.ToString() });
            listView_room.Items.Add(newItem);
            listView_room.Items[0].Selected = true;
        }

        void RemoveRoomListUI(int key)
        {
            roomManager.roomDictionary.Remove(key);

            for (int i = 0; i < listView_room.Items.Count; i++)
            {
                ListViewItem item = listView_room.Items[i];

                if (Int32.Parse(item.SubItems[0].Text) == key)
                    listView_room.Items.Remove(item);
            }
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
            listView_room.Enabled = false;

            // title, number 입력
            textBox_roomTitle.Text = listView_room.SelectedItems[0].SubItems[1].Text;
            textBox_roomNumber.Text = listView_room.SelectedItems[0].SubItems[0].Text;
        }

        void DisableRoomUI()
        {
            listBox_chat.Items.Clear();
            listBox_user.Items.Clear();
            textBox_roomTitle.Clear();
            textBox_roomNumber.Clear();

            button_RoomLeave.Enabled = false;
            listView_room.Enabled = true;
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
        void AddChatMsgUI()
        {
            CheckChatLimitCount();
            listBox_chat.Items.Add(textBox_chat.Text);
            chatDrawIndexFlags[listBox_chat.Items.Count - 1] = ChatDrawingType.RESPONSE_CHAT;
            textBox_chat.Clear();
        }

        void AddChatMsgUI(SC_CHAT_NOTIFY chatNotifyPacket)
        {
            CheckChatLimitCount();
            listBox_chat.Items.Add(chatNotifyPacket.userID + ": ");
            chatDrawIndexFlags[listBox_chat.Items.Count - 1] = ChatDrawingType.NOTIFY_ID;
            listBox_chat.Items.Add(chatNotifyPacket.message);
            chatDrawIndexFlags[listBox_chat.Items.Count - 1] = ChatDrawingType.NOTIFY_CHAT;
        }

        void SetLoginStateUI()
        {
            button_login.Text = "Logout";
            userID = textBox_ID.Text;

            textBox_ID.Enabled = false;
            textBox_password.Enabled = false;
            button_register.Enabled = false;

            button_RoomCreate.Enabled = true;
            button_RoomEnter.Enabled = true;

            IsActivatedLogin = true;
            button_isConnect.Enabled = false;
        }

        void DisableLoginStateUI()
        {
            button_login.Text = "Login";
            userID = null;
            textBox_ID.Text = "";
            textBox_ID.Enabled = true;

            textBox_password.Enabled = true;
            textBox_password.Text = "";
            button_register.Enabled = true;

            // listview Clear
            listView_room.Items.Clear();
            button_RoomCreate.Enabled = false;
            button_RoomEnter.Enabled = false;

            IsActivatedLogin = false;
            button_isConnect.Enabled = true;
        }

        private void listView_room_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
        {
            e.NewWidth = listView_room.Columns[e.ColumnIndex].Width;
            e.Cancel = true;
        }

        private void changeStatusLabelUI(string status)
        {
            label_Status.Text = "Status : " + status;
        }
    }
}
