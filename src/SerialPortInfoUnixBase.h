/**
 * @file SerialPortInfoUnixBase.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPortInfo unix class unix串口信息辅助类基类
 * @todo Not implemented 未实现
 * @date 2018-12-10
 * 
 * @copyright Copyright (c) 2019 - itas109
 * 
 */
#ifndef __CSERIALPORTINFOUNIXBASE_H__
#define __CSERIALPORTINFOUNIXBASE_H__

#include "SerialPortInfoBase.h"

/**
 * @brief the CSerialPortInfo unix class unix串口信息辅助类基类
 * 
 */
class CSerialPortInfoUnixBase :	public CSerialPortInfoBase
{
public:
	CSerialPortInfoUnixBase();
	~CSerialPortInfoUnixBase();

    static std::list<std::string> availablePorts();
private:
    //https://stackoverflow.com/questions/2530096/how-to-find-all-serial-devices-ttys-ttyusb-on-linux-without-opening-them
    static std::list<std::string> getComList();
};
#endif//__CSERIALPORTINFOUNIXBASE_H__
