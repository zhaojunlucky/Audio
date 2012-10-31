using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using textonglass;
using System.Drawing;

namespace ZLPInstall
{
    public partial class AeroLabel : Label
    {
        private Control ctr;
        public AeroLabel()
        {
            InitializeComponent();
            //this.Visible = false;
            //this.Paint += new PaintEventHandler(AreoLabel_Paint);
        }

  
        //void AreoLabel_Paint(object sender, PaintEventArgs e)
        //{
        //    if (null != ctr)
        //    {
        //        GlassText gt = new GlassText();
        //        gt.FillBlackRegion(ctr.CreateGraphics(), ctr.ClientRectangle);
        //        gt.DrawTextOnGlass(ctr.Handle, this.Text, this.Font, this.Bounds, 10);
        //    }
        //    //e.Graphics.Clear(Color.Transparent);
        //}
        [CategoryAttribute("Other")]
        public Control PControl
        {
            set
            {
                ctr = value;
                this.Refresh();
            }
        }

        private void AeroLabel_Paint(object sender, PaintEventArgs e)
        {
            if (null != ctr)
            {
                GlassText gt = new GlassText();
                //gt.FillBlackRegion(ctr.CreateGraphics(), this.ClientRectangle);
                //Brush b = Brushes.Transparent;
                //e.Graphics.Clear(Color.Transparent);
                //e.Graphics.FillRectangle(b, this.Bounds);
                gt.DrawTextOnGlass(ctr.Handle, this.Text, this.Font, this.Bounds, 10);
            }

        }


    }
}
