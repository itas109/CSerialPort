/**
 * @file Program.cs
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief CSharp CSerialPort Example  C#的CSerialPort示例程序
 */
using System;
using itas109;

public class Program
{
    static void Main()
    {
        CSerialPort sp = new CSerialPort();
        Console.WriteLine("Version: {0}\n", sp.getVersion());

        CSerialPortListener listener = new MyListener(sp);

        SerialPortInfoVector spInfoVec = new SerialPortInfoVector();
        spInfoVec = CSerialPortInfo.availablePortInfos();

        Console.WriteLine("Available Friendly Ports:");

        for (int i = 1; i <= spInfoVec.Count; ++i)
        {
            Console.WriteLine("{0} - {1} {2}", i, spInfoVec[i - 1].portName, spInfoVec[i - 1].description);
        }

        if (spInfoVec.Count == 0)
        {
            Console.WriteLine("No Valid Port");
        }
        else
        {
            int input = -1;
            do
            {
                Console.WriteLine("Please Input The Index Of Port(1 - {0})", spInfoVec.Count);

                if (!int.TryParse(Console.ReadLine(), out input))
                {
                    continue;
                }

                if (input >= 1 && input <= spInfoVec.Count)
                {
                    break;
                }
            } while (true);

            string portName = spInfoVec[input - 1].portName;
            Console.WriteLine("Port Name: {0}", portName);

            sp.init(portName,             // windows:COM1 Linux:/dev/ttyS0
                    9600,                 // baudrate
                    Parity.ParityNone,    // parity
                    DataBits.DataBits8,   // data bit
                    StopBits.StopOne,     // stop bit
                    FlowControl.FlowNone, // flow
                    4096                  // read buffer size
                    );
            sp.setReadIntervalTimeout(0); // read interval timeout

            sp.open();

            Console.WriteLine("Open {0} {1}", portName, sp.isOpen() ? "Success" : "Failed");

            // connect for read
            sp.connectReadEvent(listener);

            // write hex data
            sp.writeData(new byte[] { 0x31, 0x32, 0x33, 0x34, 0x35 }, 5);

            // write str data
            sp.writeData(System.Text.Encoding.Default.GetBytes("itas109"), 7);
        }

        for (; ; ) { }
    }
}

public class MyListener : CSerialPortListener
{
    public MyListener(CSerialPort sp)
      : base()
    {
        m_sp = sp;
    }

    public override void onReadEvent(string portName, uint readBufferLen)
    {
        if (readBufferLen > 0)
        {
            // read
            byte[] data = new byte[readBufferLen];
            int recLen = m_sp.readAllData(data);

            string str = System.Text.Encoding.Default.GetString(data);
            Console.WriteLine("{0} - Count: {1}, Length: {2}, Str: {3}, Hex: {4}", portName, ++countRead, recLen, str, BitConverter.ToString(data));

            // return receive data
            m_sp.writeData(data, recLen);
        }
    }

    CSerialPort m_sp;
    int countRead = 0;
}
