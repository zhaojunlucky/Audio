using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;

namespace LRC
{
    public class PipeServer
    {
        private MainWindow main;
        public NamedPipeServerStream serverStream = null;
        public bool running = true;
        public PipeServer(MainWindow m)
        {
            main = m;
        }

        public void StartPipeServer()
        {
            serverStream = new NamedPipeServerStream("zjlrcpipe",
                PipeDirection.InOut,
                10,
                PipeTransmissionMode.Message,
                PipeOptions.None);
            serverStream.ReadMode = PipeTransmissionMode.Message;
            Byte[] bytes = new Byte[1024];
            UTF8Encoding encoding = new UTF8Encoding();
            int numBytes;
            while (running)
            {

                serverStream.WaitForConnection();
                string message = "";
                do
                {
                    numBytes = serverStream.Read(bytes, 0, bytes.Length);

                    message += encoding.GetString(bytes, 0, numBytes);
                } while (!serverStream.IsMessageComplete);

                string[] pas = message.Split('|');
                if (null != pas && pas.Length >= 3)
                {
                    if (!(pas[0] == "exit" && pas[1] == "exit" && pas[2] == "exit"))
                        main.Dispatcher.Invoke(main.searchLrcByZLP, pas[0], pas[1], pas[2]);
                    else
                        running = false;
                }
                
                serverStream.Disconnect();

            }
            serverStream.Dispose();     
        }

        public static void SendPipeMessgae(String mes)
        {
            using (NamedPipeClientStream clientStream = new NamedPipeClientStream("zjlrcpipe"))
            {
                // connect to the pipe sever
                clientStream.Connect();
                UTF8Encoding encoding = new UTF8Encoding();
                Byte[] bytes = encoding.GetBytes(mes);
                clientStream.Write(bytes, 0, bytes.Length);
            }

        }
    }
}
