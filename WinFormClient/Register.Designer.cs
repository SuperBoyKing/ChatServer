
namespace WinFormClient
{
    partial class Register
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.textBox_registerPW = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_registerID = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.button_registerCancel = new System.Windows.Forms.Button();
            this.button_Register = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBox_registerPW
            // 
            this.textBox_registerPW.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBox_registerPW.Location = new System.Drawing.Point(103, 106);
            this.textBox_registerPW.Name = "textBox_registerPW";
            this.textBox_registerPW.PasswordChar = '*';
            this.textBox_registerPW.Size = new System.Drawing.Size(199, 23);
            this.textBox_registerPW.TabIndex = 7;
            this.textBox_registerPW.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_registerPW_KeyPress);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(19, 109);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(68, 15);
            this.label2.TabIndex = 6;
            this.label2.Text = "password : ";
            // 
            // textBox_registerID
            // 
            this.textBox_registerID.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBox_registerID.Location = new System.Drawing.Point(103, 62);
            this.textBox_registerID.Name = "textBox_registerID";
            this.textBox_registerID.Size = new System.Drawing.Size(199, 23);
            this.textBox_registerID.TabIndex = 5;
            this.textBox_registerID.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_registerID_KeyPress);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(18, 65);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(30, 15);
            this.label3.TabIndex = 4;
            this.label3.Text = "ID : ";
            // 
            // button_registerCancel
            // 
            this.button_registerCancel.Location = new System.Drawing.Point(179, 171);
            this.button_registerCancel.Name = "button_registerCancel";
            this.button_registerCancel.Size = new System.Drawing.Size(88, 31);
            this.button_registerCancel.TabIndex = 9;
            this.button_registerCancel.Text = "Cancel";
            this.button_registerCancel.UseVisualStyleBackColor = true;
            this.button_registerCancel.Click += new System.EventHandler(this.button_registerCancel_Click);
            // 
            // button_Register
            // 
            this.button_Register.Location = new System.Drawing.Point(54, 171);
            this.button_Register.Name = "button_Register";
            this.button_Register.Size = new System.Drawing.Size(88, 31);
            this.button_Register.TabIndex = 8;
            this.button_Register.Text = "Ok";
            this.button_Register.UseVisualStyleBackColor = true;
            this.button_Register.Click += new System.EventHandler(this.button_Register_Click);
            // 
            // Register
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(326, 233);
            this.Controls.Add(this.button_registerCancel);
            this.Controls.Add(this.button_Register);
            this.Controls.Add(this.textBox_registerPW);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_registerID);
            this.Controls.Add(this.label3);
            this.Name = "Register";
            this.Text = "Register";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBox_registerPW;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox_registerID;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button button_registerCancel;
        private System.Windows.Forms.Button button_Register;
    }
}