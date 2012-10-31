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
    /// Interaction logic for FolderBrowerDialog.xaml
    /// </summary>
    public partial class FolderBrowerDialog : Window
    {
        public struct _UserSelectStatus
        {
            public bool cancel;
            public string selectedPath;        
        }

        private _UserSelectStatus userSelectStatus;
        public FolderBrowerDialog(Window window)
        {
            InitializeComponent();
            this.Owner = window;
            userSelectStatus = new _UserSelectStatus();
            userSelectStatus.cancel = true;
            userSelectStatus.selectedPath = "";
            MemoryStream stream = new MemoryStream();
            Properties.Resources.lrc.Save(stream, System.Drawing.Imaging.ImageFormat.Png);
            ImageSourceConverter imageSourceConverter = new ImageSourceConverter();
            this.Icon = (ImageSource)imageSourceConverter.ConvertFrom(stream);
        }

        public _UserSelectStatus UserSelectStatus
        {
            get
            {
                return this.userSelectStatus;
            }
        }

        private void ok_Click(object sender, RoutedEventArgs e)
        {
            userSelectStatus.cancel = false;
            userSelectStatus.selectedPath = this.directoryTreeView1.SelectPath;
            this.Close();
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
