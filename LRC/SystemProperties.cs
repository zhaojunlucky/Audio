using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace LRC
{
    public class SystemProperties
    {
        [DllImport("shell32.dll", SetLastError = true)]
        extern public static bool ShellExecuteEx(ref ShellExecuteInfo lpExecInfo);

        [Serializable]
        public struct ShellExecuteInfo
        {
            public int Size;
            public uint Mask;
            public IntPtr hwnd;
            public string Verb;
            public string File;
            public string Parameters;
            public string Directory;
            public uint Show;
            public IntPtr InstApp;
            public IntPtr IDList;
            public string Class;
            public IntPtr hkeyClass;
            public uint HotKey;
            public IntPtr Icon;
            public IntPtr Monitor;
        }

        public const int SEE_MASK_INVOKEIDLIST = 0xC;
        public const int SEE_MASK_NOCLOSEPROCESS = 0x40;
        public const int SEE_MASK_FLAG_NO_UI = 0x400;


        public static void ShowProperties(IntPtr hwnd, string FileName)
        {
            try
            {
                ShellExecuteInfo SEI = new ShellExecuteInfo();
                SEI.Mask = SEE_MASK_NOCLOSEPROCESS + SEE_MASK_INVOKEIDLIST + SEE_MASK_FLAG_NO_UI;
                SEI.hwnd = hwnd;
                SEI.Verb = "properties";
                SEI.File = FileName;
                SEI.Show = 1;
                SEI.Size = System.Runtime.InteropServices.Marshal.SizeOf(SEI);
                bool r = ShellExecuteEx(ref SEI);
            }
            catch
            {
                
            }
        }
    }
}
