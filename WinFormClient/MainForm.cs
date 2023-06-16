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
        static extern void Disconnect();

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool SendConnect();

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomListPacket();

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRegisterAccount([MarshalAs(UnmanagedType.LPStr)] string id, int idSize, [MarshalAs(UnmanagedType.LPStr)] string pwd, int pwdSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendLoginPacket([MarshalAs(UnmanagedType.LPStr)] string id, int idSize, [MarshalAs(UnmanagedType.LPStr)] string pwd, int pwdSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendLogoutPacket([MarshalAs(UnmanagedType.LPStr)] string id, int idSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendChatPacket([MarshalAs(UnmanagedType.LPStr)] string str, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomOpenPacket([MarshalAs(UnmanagedType.LPStr)] string str, int titleSize, int userCount);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomEnterPacket(int number);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomLeavePacket(int number);


        // 로컬 ChatApplication 정보
        string userID = null;
        string roomTitle = null;
        int roomMaxUserCount = 0;
        RoomManager roomManager = new RoomManager();
        ChatDrawingType[] chatDrawIndexFlags = new ChatDrawingType[512];

        // ChatApplication SubForm
        RoomCreator roomCreator = new RoomCreator();
        Register registerAccount = new Register();

        // flag
        bool IsActivatedLogin = false;
        bool IsActivatedConnect = false;
        bool IsActiveEnterdRoom = false;

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
            if (!IsActivatedConnect)
            {
                SetStatusLabelUI("Connecting to Chat Server...");
                ChatClientStart(textBox_IP.Text, Int16.Parse(textBox_port.Text));

                if(!SendConnect())
                {
                    Disconnect();
                    SetStatusLabelUI("Server session not connected.");
                }
            }
            else
            {
                Disconnect();
                DisableConnectionUI();
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
            SendChatPacket(textBox_chat.Text, textBox_chat.Text.Length);
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
            if (listView_room.Items.Count != 0 && !IsActiveEnterdRoom)
                button_RoomEnter.Enabled = true;
            else
                button_RoomEnter.Enabled = false;

            ProcessPacket();
        }

        void SetConnectionUI()
        {
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
            SetStatusLabelUI("Connected Chat Server");
        }

        void DisableConnectionUI()
        {
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
            SetStatusLabelUI("DisConnected");
        }

        void AddRoomListUI(Room room)
        {
            if (!roomManager.roomDictionary.ContainsKey(room.number))
            {
                roomManager.roomDictionary.Add(room.number, room);
                ListViewItem newItem = new ListViewItem(new string[] { room.number.ToString(), room.title, room.maxUserCount.ToString() });
                listView_room.Items.Add(newItem);
                listView_room.Items[0].Selected = true;
            }
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
            IsActiveEnterdRoom = true;
            button_RoomEnter.Enabled = false;
            listView_room.Enabled = false;

            // 로그인/로그아웃 비활성화
            button_login.Enabled = false;

            // title, number 입력
            textBox_roomTitle.Text = listView_room.SelectedItems[0].SubItems[1].Text;
            textBox_roomNumber.Text = listView_room.SelectedItems[0].SubItems[0].Text;
        }

        void DisableRoomUI()
        {
            // Chat UI 비활성화
            button_chat.Enabled = false;
            textBox_chat.Enabled = false;
            listBox_chat.Enabled = false;
            listBox_user.Enabled = false;

            // Leave 버튼 비활성화
            button_RoomLeave.Enabled = false;
            IsActiveEnterdRoom = false;
            button_RoomEnter.Enabled = true;
            listView_room.Enabled = true;

            // Room Clear
            listBox_chat.Items.Clear();
            listBox_user.Items.Clear();
            textBox_roomTitle.Clear();
            textBox_roomNumber.Clear();

            // 로그인/로그아웃 활성화
            button_login.Enabled = true;

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
            roomManager.roomDictionary.Clear();

            IsActivatedLogin = false;
            button_isConnect.Enabled = true;
        }

        private void listView_room_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
        {
            e.NewWidth = listView_room.Columns[e.ColumnIndex].Width;
            e.Cancel = true;
        }

        private void SetStatusLabelUI(string status)
        {
            label_Status.Text = "Status : " + status;
        }

        private void btnEnther_Click(object sender, EventArgs e)
        {
            if (textBox_chat.Text == "")
            {
                MessageBox.Show("Please insert chat text", "Info", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                textBox_chat.Focus();
            }
            else
            {
                SendChatPacket(textBox_chat.Text, textBox_chat.Text.Length);
                button_chat.Enabled = false;
            }
        }

        // 키 입력 제한
        private void textBox_ID_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsLetter(e.KeyChar) || Char.IsNumber(e.KeyChar)) && e.KeyChar != 8)
            {
                e.Handled = true;
            }
        }

        private void textBox_password_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsLetter(e.KeyChar) || Char.IsNumber(e.KeyChar)) && e.KeyChar != 8)
            {
                e.Handled = true;
            }
        }

        private void textBox_chat_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                btnEnther_Click(sender, e);
            }
        }

        private void textBox_IP_KeyPress(object sender, KeyPressEventArgs e)
        {
            int iPos = 0;               // IP 구역의 현재 위치
            int iDelimitNumber = 0;     // IP 구역의 갯수

            int iLength = textBox_IP.Text.Length;
            int iIndex = textBox_IP.Text.LastIndexOf(".");

            int iIndex2 = -1;
            while (true)
            {
                iIndex2 = textBox_IP.Text.IndexOf(".", iIndex2 + 1);
                if (iIndex2 == -1)
                    break;

                ++iDelimitNumber;
            }

            // 숫자키와 백스페이스, '.' 만 입력 가능
            if ((e.KeyChar < 48 || e.KeyChar > 57) &&
            e.KeyChar != 8 && e.KeyChar != '.')
            {
                e.Handled = true;
                return;
            }

            if (e.KeyChar != 8)
            {
                if (e.KeyChar != '.')
                {
                    if (iIndex > 0)
                        iPos = iLength - iIndex;
                    else
                        iPos = iLength + 1;

                    if (iPos == 3)
                    {
                        // 255 이상 체크
                        string strTmp = textBox_IP.Text.Substring(iIndex + 1) + e.KeyChar;
                        if (Int32.Parse(strTmp) > 255)
                        {
                            MessageBox.Show("255를 넘길수 없습니다.", "오류");
                            e.Handled = true;
                            return;
                        }
                        else
                        {
                            // 3자리가 넘어가면 자동으로 .을 찍어준다
                            if (iDelimitNumber < 3)
                            {
                                textBox_IP.AppendText(e.KeyChar.ToString());
                                textBox_IP.AppendText(".");
                                iDelimitNumber++;
                                e.Handled = true;
                                return;
                            }
                        }
                    }

                    // IP 마지막 4자리째는 무조건 무시
                    if (iPos == 4)
                    {
                        e.Handled = true;
                        return;
                    }
                }
                else
                {
                    // 아이피가 3구역 이상 쓰였으면, 이후 키는 무시한다
                    if (iDelimitNumber + 1 > 3)
                    {
                        MessageBox.Show("IP 주소가 정확하지 않습니다.", "오류");
                        e.Handled = true;
                        return;
                    }
                    else
                    {
                        // 연속으로 .을 찍었으면 오류
                        if (textBox_IP.Text.EndsWith("."))
                        {
                            MessageBox.Show("IP 주소가 정확하지 않습니다.", "오류");
                            e.Handled = true;
                            return;
                        }
                        else
                            iDelimitNumber++;
                    }
                }
            }
        }

        private void textBox_port_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != 8) 
            {
                e.Handled = true;
            }
            else
            {
                if (Char.IsNumber(e.KeyChar))
                {
                    string portText = textBox_port.Text + e.KeyChar.ToString();

                    if (Int32.Parse(portText) > UInt16.MaxValue)
                    {
                        e.Handled = true;
                    }
                }   
            }
        }
    }
}
