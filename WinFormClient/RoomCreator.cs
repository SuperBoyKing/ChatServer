using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace WinFormClient
{
    public partial class RoomCreator : Form
    {
        public string returnTitle { get; set; }
        public int  returnUserCount { get; set; }

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomOpenPacket([MarshalAs(UnmanagedType.LPStr)] string title, int titleSize, int userCount);

        public RoomCreator()
        {
            InitializeComponent();
        }

        private void button_create_Click(object sender, EventArgs e)
        {
            returnTitle = textBox_title.Text;
            returnUserCount = Int32.Parse(textBox_userCount.Text);

            if (returnTitle != null && returnUserCount > 1)
            {
                //SendRoomOpenPacket(returnTitle, returnTitle.Length, returnUserCount);
            }

            this.DialogResult = DialogResult.OK;
            Close();
        }

        private void button_cancel_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
