using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace LRC.Shell
{
    public class API
    {

        [DllImport("User32.dll")]
        public static extern IntPtr GetSystemMenu(IntPtr hWnd, Int32 bRevert);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern int AppendMenu(IntPtr hMenu, int Flagsw, int IDNewItem, string lpNewItem);
    }
}
