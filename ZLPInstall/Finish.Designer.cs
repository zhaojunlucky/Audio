namespace ZLPInstall
{
    partial class Finish
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.startApp = new System.Windows.Forms.CheckBox();
            this.finishBtn = new System.Windows.Forms.Button();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Font = new System.Drawing.Font("Calibri", 21.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.Color.Fuchsia;
            this.label1.Location = new System.Drawing.Point(107, 186);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(292, 43);
            this.label1.TabIndex = 8;
            this.label1.Text = "Installation is complete";
            this.label1.UseCompatibleTextRendering = true;
            // 
            // startApp
            // 
            this.startApp.AutoSize = true;
            this.startApp.BackColor = System.Drawing.Color.Transparent;
            this.startApp.Font = new System.Drawing.Font("Calibri", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.startApp.Location = new System.Drawing.Point(181, 248);
            this.startApp.Name = "startApp";
            this.startApp.Size = new System.Drawing.Size(128, 19);
            this.startApp.TabIndex = 9;
            this.startApp.Text = "Now start the ZLPlayer";
            this.startApp.UseCompatibleTextRendering = true;
            this.startApp.UseVisualStyleBackColor = false;
            this.startApp.Visible = false;
            // 
            // finishBtn
            // 
            this.finishBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.finishBtn.Location = new System.Drawing.Point(205, 273);
            this.finishBtn.Name = "finishBtn";
            this.finishBtn.Size = new System.Drawing.Size(80, 39);
            this.finishBtn.TabIndex = 10;
            this.finishBtn.Text = "Finish";
            this.finishBtn.UseCompatibleTextRendering = true;
            this.finishBtn.UseVisualStyleBackColor = true;
            // 
            // pictureBox2
            // 
            this.pictureBox2.BackColor = System.Drawing.Color.Transparent;
            this.pictureBox2.Image = global::ZLPInstall.Properties.Resources.ZJA;
            this.pictureBox2.Location = new System.Drawing.Point(181, 33);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(130, 130);
            this.pictureBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictureBox2.TabIndex = 7;
            this.pictureBox2.TabStop = false;
            // 
            // Finish
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Transparent;
            this.Controls.Add(this.finishBtn);
            this.Controls.Add(this.startApp);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.pictureBox2);
            this.Name = "Finish";
            this.Size = new System.Drawing.Size(514, 352);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBox2;
        public System.Windows.Forms.Label label1;
        public System.Windows.Forms.Button finishBtn;
        public System.Windows.Forms.CheckBox startApp;
    }
}
