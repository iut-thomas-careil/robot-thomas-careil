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
           
            serialPort1 = new ReliableSerialPort("COM4", 115200, Parity.None, 8,StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            //serialPort1.Open();
            DispatcherTimer timerAffichage;
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
        
            int x = 0;
            /*textBoxReception.Text += robot.receivedText;
            robot.receivedText = "";*/
            while (robot.byteListReceived.Count > 0) {
                byte byteReceived = robot.byteListReceived.Dequeue();
                DecodeMessage(byteReceived);

                //string receivedText = "0x" + byteReceived.ToString("X2") + " ";
                //textBoxReception.Text += receivedText;
            }
        }

        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            for (int i=0; i < e.Data.Length; i++) 
            { 
                robot.byteListReceived.Enqueue(e.Data[i]);
            }
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

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            
            int i;
            byte[] byteList = new byte[20];
            for (i = 0; i < 20; i++)
            {
                byteList[i] = (byte)(2 * i);
            }
            serialPort1.Write(byteList, 0, byteList.Length);
        }        

        void SendMessage() {
            /*textBoxEmission.Text = textBoxEmission.Text.TrimEnd('\n');
            //textBoxReception.Text = textBoxReception.Text + "Recu :" + message + "\n";
            serialPort1.WriteLine(textBoxEmission.Text);
            textBoxEmission.Text = "";*/
            textBoxEmission.Text = textBoxEmission.Text.TrimEnd('\n');
            string message = textBoxEmission.Text;
            byte[] array = Encoding.ASCII.GetBytes(message);
            UartEncodeAndSendMessage(0x0080, array.Length, array);
            textBoxEmission.Text = "";

        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checkSum = 0xFE;
            checkSum ^= (byte)(msgFunction >> 8);
            checkSum ^= (byte)(msgFunction >> 0);
            checkSum ^= (byte)(msgPayloadLength >> 8);
            checkSum ^= (byte)(msgPayloadLength >> 0);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                checkSum ^= msgPayload[i]; 
            }
            return checkSum;
        }

        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            int i;
            byte[] trame = new byte[6 + msgPayload.Length];
            trame[0] = 0xFE;
            trame[1] = (byte)(msgFunction >> 8);
            trame[2] = (byte)(msgFunction >> 0);
            trame[3] = (byte)(msgPayloadLength >> 8);
            trame[4] = (byte)(msgPayloadLength >> 0);
            for (i = 0; i < msgPayloadLength; i++)
            {
                trame[5+i] ^= msgPayload[i];
            }
            trame[5 + i] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
            serialPort1.Write(trame,0,trame.Length);
        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;

        public void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;

                case StateReception.FunctionMSB:
                    msgDecodedFunction = (byte)(c << 8);
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgDecodedFunction += (byte)(c << 0);
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = (byte)(c << 8);
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength = (byte)(c << 0);
                    if (msgDecodedPayloadLength == 0)
                        rcvState = StateReception.CheckSum;
                    else if (msgDecodedPayloadLength >= 1024)
                        rcvState = StateReception.Waiting;
                    else
                    {
                        rcvState = StateReception.Payload;
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        msgDecodedPayloadIndex = 0;
                    }
                    break;

                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                        rcvState = StateReception.CheckSum;
                    break;

                case StateReception.CheckSum:
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == c)
                    {
                        textBoxReception.Text = textBoxReception.Text + "Success, on a un message valide" + "\n";
                    }
                    else
                    {
                        textBoxReception.Text = textBoxReception.Text + "Sorry ! Try again !" + "\n";
                    }
                    rcvState = StateReception.Waiting;
                    break;

                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

            public enum commande: int {
            Texte=0x0080,
            Led=0x0020,
            DistanceTelemetre=0x0030,
            ConsigneVitesse=0x0040,

        }
            void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[ ] msgPayload) {
            
            
            }

        }
    }


  


