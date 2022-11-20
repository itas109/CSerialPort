#***************************************************************************
# @file main.py
# @author itas109 (itas109@qq.com) \n\n
# Blog : https://blog.csdn.net/itas109 \n
# Github : https://github.com/itas109 \n
# Gitee : https://gitee.com/itas109 \n
# QQ Group : 129518033
# @brief Python CSerialPort Example  Python的CSerialPort示例程序
############################################################################
import cserialport

def main():
    sp = cserialport.CSerialPort()
    print("Version: %s\n" %(sp.getVersion()))

    listener = MyListener(sp).__disown__()

    spInfoVec = cserialport.CSerialPortInfo.availablePortInfos()
    print("Available Friendly Ports:")
    for index, spInfo in enumerate(spInfoVec, start=1):
        print("%d - %s %s" %(index, spInfo.portName, spInfo.description))

    if len(spInfoVec) == 0:
        print("No Valid Port")
    else:
        while True:
            print("Please Input The Index Of Port(1 - %d)" %(len(spInfoVec)))
            myInput=int(input())
            if myInput >= 1 and myInput <= len(spInfoVec):
                break
        portName=spInfoVec[myInput-1].portName
        print("Port Name: %s" %(portName))

        sp.init(portName,               # windows:COM1 Linux:/dev/ttyS0
                9600,                   # baudrate
                cserialport.ParityNone, # parity
                cserialport.DataBits8,  # data bit
                cserialport.StopOne,    # stop bit
                cserialport.FlowNone,   # flow
                4096                    # read buffer size
                )
        sp.setReadIntervalTimeout(0); # read interval timeout
        sp.open()
        print("Open %s %s" %(portName, "Success" if sp.isOpen() else "Failed"))

        # connect for read
        sp.connectReadEvent(listener)

        # write hex data
        hex = cserialport.malloc_void(5)
        cserialport.memmove(hex, '\x31\x32\x33\x34\x35')
        sp.writeData(hex, 5)
        cserialport.free_void(hex)

        # write str data
        data = cserialport.malloc_void(7)
        cserialport.memmove(data, "itas109")
        sp.writeData(data, 7)
        cserialport.free_void(data)

        while True:
            pass

def byte2hexStr(data):
    ch = ['0x%02X' % ord(i) for i in data]
    return " ".join(ch)

class MyListener(cserialport.CSerialPortListener):
    def __init__(self, sp):
        MyListener.countRead=0
        MyListener.sp=sp
        cserialport.CSerialPortListener.__init__(self)

    def onReadEvent(self, portName, readBufferLen):
        data = cserialport.malloc_void(readBufferLen)
        recLen=MyListener.sp.readData(data,readBufferLen)
        str=cserialport.cdata(data, readBufferLen)
        MyListener.countRead+=1
        print("%s - Count: %d, Length: %d, Str: %s, Hex: %s" %(portName, MyListener.countRead, recLen, str, byte2hexStr(str)))
        MyListener.sp.writeData(data, readBufferLen)
        cserialport.free_void(data)

if __name__ == '__main__':
	main()