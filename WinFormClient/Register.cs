using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WinFormClient
{
    public partial class Register : Form
    {
        public string returnID { get; set; }
        public string returnPW { get; set; }

        public Register()
        {
            InitializeComponent();
        }

        private void button_Register_Click(object sender, EventArgs e)
        {

            if (string.IsNullOrEmpty(textBox_registerID.Text) || string.IsNullOrEmpty(textBox_registerPW.Text))
            {
                this.DialogResult = DialogResult.None;
            }
            else
            {
                returnID = textBox_registerID.Text;
                returnPW = textBox_registerPW.Text;
                this.DialogResult = DialogResult.OK;
            }

            Close();
        }

        private void button_registerCancel_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
