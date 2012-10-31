using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows;

namespace LRC
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        internal static string args = "";
        public App()
        {
            AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(CurrentDomain_UnhandledException);
            this.DispatcherUnhandledException += new System.Windows.Threading.DispatcherUnhandledExceptionEventHandler(App_DispatcherUnhandledException);

        }

        protected override void OnStartup(StartupEventArgs e)
        {
            if (CheckAnotherInstance() && e.Args.Length >= 1)
            {

                PipeServer.SendPipeMessgae(e.Args[0]);
                Thread.Sleep(500);
                System.Environment.Exit(0);
            }
            if(e.Args.Length >= 1)
                args = e.Args[0];
            base.OnStartup(e);
        }


        private bool CheckAnotherInstance()
        {
            bool isFirst;
            System.Threading.Mutex mtx = new System.Threading.Mutex
                                                                (false, "zjdd ", out   isFirst);
            mtx.Close();
            return !isFirst;
        }


        void App_DispatcherUnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e)
        {
            try
            {
                Exception ex = e.Exception;
                MessageBox.Show(ex.Message + Environment.NewLine + ex.StackTrace);
            }
            catch
            {
                MessageBox.Show("不可恢复的异常，应用程序将退出！");
            }
        }

        void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            try
            {
                Exception ex = e.ExceptionObject as Exception;
                MessageBox.Show(ex.Message + Environment.NewLine + ex.StackTrace);
            }
            catch
            {
                MessageBox.Show("不可恢复的异常，应用程序将退出！");
            }
        }
    }
}
