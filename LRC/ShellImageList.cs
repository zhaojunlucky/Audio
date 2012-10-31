using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Windows.Media;
using System.IO;
using Microsoft.WindowsAPICodePack.Shell;

namespace LRC
{
    public class ShellImageList
    {
        public ShellImageList()
        {

        }

        public ImageSource GetDirectoryImage(string path, bool small)
        {
            if (Directory.Exists(path))
            {
                ShellFileSystemFolder shellFileSystemFolder = ShellFileSystemFolder.FromFolderPath(path);
                return small ? shellFileSystemFolder.Thumbnail.SmallBitmapSource : shellFileSystemFolder.Thumbnail.LargeBitmapSource;
            }
            else
            {
                ShellObject shellObject = ShellFileSystemFolder.FromParsingName(path);
                return small ? shellObject.Thumbnail.SmallBitmapSource : shellObject.Thumbnail.LargeBitmapSource;
            }
        }
    }
}
