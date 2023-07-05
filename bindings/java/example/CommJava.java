/**
 * @file CommJava.java
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief Java CSerialPort Example  Java的CSerialPort示例程序
 */
import java.util.Scanner;

import com.itas109.cserialport.*;

public class CommJava {
    static {
        try {
            System.loadLibrary("cserialport");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load\n" + e);
            System.exit(1);
        }
    }

    public static void main(String[] args) {
        CSerialPort sp = new CSerialPort();
        System.out.printf("Version: %s\n", sp.getVersion());

        MyListener listener = new MyListener(sp);

        SerialPortInfoVector spInfoVec = new SerialPortInfoVector();
        spInfoVec = CSerialPortInfo.availablePortInfos();

        System.out.println("Available Friendly Ports:");

        for (int i = 1; i <= spInfoVec.size(); ++i) {
            SerialPortInfo serialPortInfo = spInfoVec.get(i - 1);
            System.out.printf("%d - %s %s\n", i, serialPortInfo.getPortName(), serialPortInfo.getDescription());
        }

        if (spInfoVec.size() == 0) {
            System.out.println("No Valid Port");
        } else {
            int input = -1;
            do {
                System.out.printf("Please Input The Index Of Port(1 - %d)\n", spInfoVec.size());

                Scanner sc = new Scanner(System.in);
                input = sc.nextInt();
                sc.close();

                if (input >= 1 && input <= spInfoVec.size()) {
                    break;
                }
            } while (true);

            String portName = spInfoVec.get(input - 1).getPortName();
            System.out.printf("Port Name: %s\n", portName);

            sp.init(portName, // windows:COM1 Linux:/dev/ttyS0
                    9600, // baudrate
                    Parity.ParityNone, // parity
                    DataBits.DataBits8, // data bit
                    StopBits.StopOne, // stop bit
                    FlowControl.FlowNone, // flow
                    4096 // read buffer size
            );
            sp.setReadIntervalTimeout(0); // read interval timeout

            sp.open();

            System.out.printf("Open %s %s\n", portName, sp.isOpen() ? "Success" : "Failed");

            // connect for read
            sp.connectReadEvent(listener);

            // write hex data
            byte[] hex = new byte[] { 0x31, 0x32, 0x33, 0x34, 0x35 };
            sp.writeData(hex, 5);

            // write str data
            byte[] data = "itas109".getBytes();
            sp.writeData(data, 7);
        }

        for (;;) {
        }
    }
}

class MyListener extends CSerialPortListener {
    public MyListener(CSerialPort sp) {
        super();

        countRead = 0;
        m_sp = sp;
    }

    public void onReadEvent(String portName, long readBufferLen) {
        byte[] data = new byte[(int) readBufferLen];
        int recLen = m_sp.readAllData(data);
        String str = new String(data);
        System.out.printf("%s - Count: %d, Length: %d, Str: %s, Hex: %s\n", portName, ++countRead, recLen, str,
                byte2hexStr(data));

        // return receive data
        m_sp.writeData(data, (int) readBufferLen);
    }

    public static String byte2hexStr(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        String ch;
        for (byte b : bytes) {
            ch = Integer.toHexString(0xFF & b);
            if (ch.length() == 1) {
                ch = "0x0" + ch;
            } else {
                ch = "0x" + ch;
            }
            sb.append(ch).append(" ");
        }
        return sb.toString();
    }

    private CSerialPort m_sp;
    private int countRead;
}
