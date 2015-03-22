using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace HorisontLevelClient
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            portsListBox.Items.Clear();
            foreach (var port in SerialPort.GetPortNames())
                portsListBox.Items.Add(port);
            portsListBox.SelectedIndex = 0;
        }

        private void BeginButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (portsListBox.SelectedValue == null)
                    throw new Exception("Видимо, ваша конфигурация аппаратного обеспечения не поддерживает параллельный интерфейс.");
                if ((Accelerometer.Thread != null && Accelerometer.Thread.ThreadState == ThreadState.Running)
                    || (Accelerometer.State()))
                    throw new Exception("Поток передачи сообщений уже запущен.");

                var portName = portsListBox.SelectedValue.ToString();
                Accelerometer.Open(portName);

                Accelerometer.Thread = new Thread(Worker);
                Accelerometer.Thread.IsBackground = true;
                Accelerometer.Thread.Start(portName);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, this.Title, MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void EndButton_Click(object sender, RoutedEventArgs e)
        {
            Accelerometer.Thread.Abort();
            Accelerometer.Close();
        }

        private void Worker(object parameter)
        {
            try
            {
                while (true)
                {
                    Dispatcher.Invoke(new Action<string>(_Render), System.Windows.Threading.DispatcherPriority.Render, Accelerometer.Read());
                }
            }
            catch (ThreadAbortException ex)
            {
                //No...
                //Thread.ResetAbort();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, this.Title, MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        int XMax, YMax, XMin, YMin;
        int zSign, sizeTicker;
        private void _Render(string text)
        {
            var s = text.Split(',').Select(i => i.Trim()).ToArray();
            var currentX = Convert.ToInt16(s[2]);
            var currentY = -Convert.ToInt16(s[3]);
            var currentZ = Convert.ToInt16(s[4]);

            if ((zSign < 0 && currentZ > 0) || (zSign == 0 && currentZ < 0))
                ++sizeTicker;
            else
                sizeTicker = 0;

            if (sizeTicker == 5)
            {
                sizeTicker = 0;
                zSign = (currentZ < 0) ? -1 : 0;
                foregroundImage.Source = new BitmapImage(new Uri((zSign < 0) ? "stm_bottom.jpg" : "stm_top.jpg", UriKind.Relative));
            }

            if (zSign == 0)
                currentX *= -1;

            meterLine.X1 = 0;
            meterLine.Y1 = 0;
            meterLine.X2 = currentX;
            meterLine.Y2 = currentY;

            AngleLabel.Content = s[0];
            DeviationLabel.Content = s[1];
            XLabel.Content = currentX;
            YLabel.Content = currentY;

            if (currentX < XMin) XMin = currentX;
            if (currentX > XMax) XMax = currentX;
            if (currentY < YMin) YMin = currentY;
            if (currentY > YMax) YMax = currentY;

            XmaxLabel.Content = XMax;
            YmaxLabel.Content = YMax;
            XminLabel.Content = XMin;
            YminLabel.Content = YMin;
        }

        private void mainCanvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            mainCanvas.SetCoordinateSystem(-128, 128, -128, 128);
        }

        private void ResetButton_Click(object sender, RoutedEventArgs e)
        {
            XMax = 0;
            YMax = 0;
            XMin = 0;
            YMin = 0;

            XmaxLabel.Content = XMax;
            YmaxLabel.Content = YMax;
            XminLabel.Content = XMin;
            YminLabel.Content = YMin;
        }
    }
}
