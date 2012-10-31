using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace ZLPInstall
{
    public class Settings
    {
        public static bool createDesktopshortcut = true;
        public static bool createStartMenu = true;
        public static bool createTaskbar = true;
        
        public static string fileAss = "";
        public static string company = "ZJSoft";
        public static string appName = "ZLPlayer";
        public static string appExeName = "ZLPlayer.exe";
        public static string unInstallExeName = "UnInstall.exe";
        public static string userStartMenu = Path.Combine( Environment.GetFolderPath(Environment.SpecialFolder.CommonStartMenu),"Programs");
        public static string appKey = "ZLPlayer";
        public static string version = "";
        public static string mutexName = "ZLPlayer";
    }
}
