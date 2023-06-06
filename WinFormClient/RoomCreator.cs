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

        public RoomCreator()
        {
            InitializeComponent();
        }

        private void button_create_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(textBox_userCount.Text) || string.IsNullOrEmpty(textBox_title.Text))
            {
                this.DialogResult = DialogResult.None;
            }
            else
            {
                returnTitle = textBox_title.Text;
                returnUserCount = Int32.Parse(textBox_userCount.Text);
                textBox_title.Text = "";
                textBox_userCount.Text = "";
                this.DialogResult = DialogResult.OK;
            }

            Close();
        }

        private void button_cancel_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
