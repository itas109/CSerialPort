/**
 * @file CSerialPortVirtual.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief CSerialPort extend class,used to manager virtual serial port CSerialPort的扩展类,用于管理虚拟串口
 */

#ifndef __CSERIALPORT_VIRTUAL_H__
#define __CSERIALPORT_VIRTUAL_H__

namespace itas109
{
class CSerialPortVirtual
{
public:
    CSerialPortVirtual();
    ~CSerialPortVirtual();

    static bool createPair(char* portName1, char* portName2);
    static bool deletePair(char* portName);

private:
    
};
} // namespace itas109
#endif //__CSERIALPORT_VIRTUAL_H__
