using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace HorisontLevelClient
{
    public static class Accelerometer
    {
        private static SerialPort _port { set; get; }
        public static Thread Thread { set; get; }

        public static void Open(string port)
        {
            _port = new SerialPort(port, 9600, Parity.None, 8, StopBits.One);
            _port.Open();
        }

        public static string Read()
        {
            if (_port.IsOpen)
                return _port.ReadLine();
            throw new Exception("Порт закрыт, нечего принять.");
        }

        public static void Close()
        {
            if (_port.IsOpen)
                _port.Close();
        }

        public static bool State()
        {
            return (_port != null) ? _port.IsOpen : false;
        }
    }
}
