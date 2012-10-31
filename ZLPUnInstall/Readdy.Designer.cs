namespace ZLPUnInstall
{
    partial class Readdy
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
            this.label2 = new System.Windows.Forms.Label();
            this.rmSetting = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.uninstall = new System.Windows.Forms.Button();
            this.cancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.SystemColors.Control;
            this.label2.Font = new System.Drawing.Font("Times New Roman", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.ForeColor = System.Drawing.Color.Fuchsia;
            this.label2.Location = new System.Drawing.Point(3, 19);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(147, 21);
            this.label2.TabIndex = 17;
            this.label2.Text = "Uninstall ZLPlayer";
            // 
            // rmSetting
            // 
            this.rmSetting.AutoSize = true;
            this.rmSetting.Location = new System.Drawing.Point(7, 66);
            this.rmSetting.Name = "rmSetting";
            this.rmSetting.Size = new System.Drawing.Size(15, 14);
            this.rmSetting.TabIndex = 18;
            this.rmSetting.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.SystemColors.Control;
            this.label1.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.Color.Fuchsia;
            this.label1.Location = new System.Drawing.Point(28, 61);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(307, 19);
            this.label1.TabIndex = 19;
            this.label1.Text = "Remove user data and settings on your computer ";
            // 
            // uninstall
            // 
            this.uninstall.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.uninstall.Location = new System.Drawing.Point(111, 293);
            this.uninstall.Name = "uninstall";
            this.uninstall.Size = new System.Drawing.Size(80, 33);
            this.uninstall.TabIndex = 20;
            this.uninstall.Text = "Uninstall";
            this.uninstall.UseVisualStyleBackColor = true;
            // 
            // cancel
            // 
            this.cancel.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cancel.Location = new System.Drawing.Point(305, 293);
            this.cancel.Name = "cancel";
            this.cancel.Size = new System.Drawing.Size(80, 33);
            this.cancel.TabIndex = 21;
            this.cancel.Text = "Cancel";
            this.cancel.UseVisualStyleBackColor = true;
            // 
            // Readdy
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.cancel);
            this.Controls.Add(this.uninstall);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.rmSetting);
            this.Controls.Add(this.label2);
            this.Name = "Readdy";
            this.Size = new System.Drawing.Size(514, 350);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.CheckBox rmSetting;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.Button uninstall;
        public System.Windows.Forms.Button cancel;
    }
}
