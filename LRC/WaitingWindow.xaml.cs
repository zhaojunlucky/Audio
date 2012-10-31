using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.IO;

namespace LRC
{
    /// <summary>
    /// Interaction logic for WaitingWindow.xaml
    /// </summary>
    public partial class WaitingWindow : Window
    {
        internal bool canCLose;
        public WaitingWindow()
        {
            InitializeComponent();
            this.Background = new SolidColorBrush(Color.FromRgb(188, 99, 163));
            MemoryStream stream = new MemoryStream();
            Properties.Resources.lrc.Save(stream, System.Drawing.Imaging.ImageFormat.Png);
            ImageSourceConverter imageSourceConverter = new ImageSourceConverter();
            this.Icon = (ImageSource)imageSourceConverter.ConvertFrom(stream);
            canCLose = false;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            e.Cancel = !canCLose;
        }
    }
}
