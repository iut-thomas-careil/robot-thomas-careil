using ExtendedSerialPort;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
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
using System.Windows.Threading;

namespace RobotInterface
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    /// 
   
    
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        
        Robot robot = new Robot();
        public MainWindow()
        {
            InitializeComponent();
           
            serialPort1 = new ReliableSerialPort("COM3", 115200, Parity.None, 8,StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
            DispatcherTimer timerAffichage;
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
         textBoxReception.Text += robot.receivedText;
         robot.receivedText = "";
        }

        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            
        }
        
        bool toggle = false;

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if (toggle)
                buttonEnvoyer.Background = Brushes.Green;
            else
                buttonEnvoyer.Background = Brushes.Red;

            toggle = !toggle;

            SendMessage();
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            if (toggle)
                buttonClear.Background = Brushes.Green;
            else
                buttonClear.Background = Brushes.Red;

            toggle = !toggle;

           textBoxReception.Text = "";

        }

        void SendMessage() {
            textBoxEmission.Text = textBoxEmission.Text.TrimEnd('\n');
            //textBoxReception.Text = textBoxReception.Text + "Recu :" + message + "\n";
            serialPort1.WriteLine(textBoxEmission.Text);
            textBoxEmission.Text = "";

        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                SendMessage();
        }
    }
}

  


