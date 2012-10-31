using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Win32;
using System.IO;

namespace ZLPInstall
{
    public class RegistryHelper
    {
        public static string FindPrevioudInstallDir(string appKey,bool x64 = false)
        {
            string dir = "";
            RegistryKey hKey = Registry.LocalMachine;
            string subKey;
            //if (x64)
                subKey = @"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\" + appKey;
            //else
                //subKey = @"SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\" + appKey;
            try
            {
                RegistryKey app = hKey.OpenSubKey(subKey);
                if (null != app)
                {
                    dir = app.GetValue("InstallLocation").ToString().Trim();//.Replace(" ", "");
                }
            }
            catch
            {
            }
            return dir;
        }

        public static string GetDefaultProgramDir(bool x64 = false)
        {
            string dir = "";
            RegistryKey hKey = Registry.LocalMachine;
            string subKey;
            //if (x64)
                subKey = @"SOFTWARE\Microsoft\Windows\CurrentVersion\" ;
            //else
                //subKey = @"SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\" ;
            RegistryKey app = hKey.OpenSubKey(subKey);
            if (null != app)
            {
                if(x64)
                    dir = app.GetValue("ProgramFilesDir (x86)").ToString().Replace("/", "\\");
                else
                    dir = app.GetValue("ProgramFilesDir").ToString().Replace("/", "\\");
                app.Close();
            }
            if (!dir.EndsWith("\\"))
                dir += "\\";
           
            return dir;
        }
        public static bool IsFileAss(string ext, string appKey)
        {
            try
            {
                RegistryKey hKey = Registry.ClassesRoot;
                RegistryKey extHkey = hKey.OpenSubKey(ext);
                if (null == extHkey)
                    return false;
                string extvalue = extHkey.GetValue("").ToString();
                if (string.IsNullOrEmpty(extvalue) || extvalue.ToLower() != appKey.ToLower())
                {
                    hKey.Close();
                    extHkey.Close();
                    return false;
                }
                extHkey.Close();
                extHkey = hKey.OpenSubKey(appKey);
                if (null == extHkey)
                    return false;
                extHkey.Close();
                hKey.Close();
            }
            catch
            {
                return false;
            }
            return true;
        }
        public static void SetUnInstallInfo(string appKey,string installDir,string exeFile,string disPlayVer,string displayName,
            string publisher,string uninstExe,long size,bool x64 = false)
        {
            RegistryKey hKey = Registry.LocalMachine;
            string subKey;
            //if (x64)
                subKey = @"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\" + appKey;
            //else
                //subKey = @"SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\" + appKey;
            RegistryKey app = hKey.CreateSubKey(subKey);
            if (null != app)
            {
                string instDate = System.DateTime.Now.Year.ToString() + System.DateTime.Now.Month.ToString() + System.DateTime.Now.Day.ToString();
                app.SetValue("InstallDate", instDate);
                app.SetValue("InstallLocation", installDir);
                app.SetValue("DisplayName", displayName);
                app.SetValue("DisplayVersion", disPlayVer);
                app.SetValue("Publisher", publisher);
                app.SetValue("UninstallString",Path.Combine(installDir,uninstExe));
                app.SetValue("EstimatedSize", Math.Round((double)size/1024.0,2), RegistryValueKind.DWord);
                app.Close();
            }
           
        }

        public static void RegistFont(string fontName,string fontFileName)
        {
            RegistryKey hKey = Registry.LocalMachine;
            RegistryKey app = hKey.CreateSubKey("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");
            if (null != app)
            {
                app.SetValue(fontName, fontFileName);
                app.Close();
            }
        }
    }
}
