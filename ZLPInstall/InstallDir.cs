using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ZLPInstall
{
    public partial class InstallDir : UserControl
    {
        public InstallDir()
        {
            InitializeComponent();
        }

        private void dir_TextChanged(object sender, EventArgs e)
        {
            Graphics g = this.CreateGraphics();
            SizeF siF = g.MeasureString(this.dir.Text , this.dir.Font);
            int tW = this.Width;
            int strW = (int)siF.Width;
            g.Dispose();
            this.dir.Location = new Point((tW - strW) / 2 -5, this.dir.Location.Y);
        }
    }
}
