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
    /// Interaction logic for SettingWindow.xaml
    /// </summary>
    public partial class SettingWindow : Window
    {
        public SettingWindow()
        {
            InitializeComponent();
            this.Background = new SolidColorBrush(Color.FromRgb(213, 133, 191));
            MemoryStream stream = new MemoryStream();
            Properties.Resources.lrc.Save(stream, System.Drawing.Imaging.ImageFormat.Png);
            ImageSourceConverter imageSourceConverter = new ImageSourceConverter();
            this.Icon = (ImageSource)imageSourceConverter.ConvertFrom(stream);
            this.textBlock1.Text = Settings.Settings.Default.lrcDirectory;
            this.textBlock1.ToolTip = this.textBlock1.Text;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Settings.Settings.Default.lrcDirectory = this.textBlock1.Text;
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            FolderBrowerDialog folderBrowserDialog = new FolderBrowerDialog(this);
            folderBrowserDialog.ShowDialog();
            LRC.FolderBrowerDialog._UserSelectStatus selectStatus = folderBrowserDialog.UserSelectStatus;
            if (!selectStatus.cancel)
            {
                this.textBlock1.Text = selectStatus.selectedPath;
                this.textBlock1.ToolTip = this.textBlock1.Text;
            }
        }
    }
}
