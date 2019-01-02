#ifndef __CSERIALPORT_GLOBAL_H__
#define __CSERIALPORT_GLOBAL_H__

//global difine of CSerialPort
//https://stackoverflow.com/questions/5188554/my-enum-is-not-a-class-or-namespace

#include "osplatformutil.h"

#ifdef I_OS_WIN
typedef __int64 int64;           /* 64 bit signed */
#define DLL_EXPORT __declspec(dllexport)
#elif defined I_OS_UNIX
typedef long long int64;           /* 64 bit signed */
#define DLL_EXPORT     __attribute__((visibility("default")))
#else
//Not support
#endif // I_OS_WIN

namespace itas109
{
		enum OperateMode	//read and write serial port mode : 
		{
			AsynchronousOperate,	//Asynchronous
			SynchronousOperate		//Synchronous
		};

		enum BaudRate
		{
			BaudRate1200 = 1200,
			BaudRate2400 = 2400,
			BaudRate4800 = 4800,
			BaudRate9600 = 9600,			    //recommend
			BaudRate19200 = 19200,
			BaudRate38400 = 38400,
			BaudRate115200 = 115200,
		};

		//by QextSerialPort
		// 5 data bits cannot be used with 2 stop bits
		// 1.5 stop bits can only be used with 5 data bits
		// 8 data bits cannot be used with space parity on POSIX systems

		enum DataBits
		{
			//windows Number of bits/byte, 4-8 
			DataBits5 = 5,			//5 data bits
			DataBits6 = 6,			//6 data bits
			DataBits7 = 7,			//7 data bits
			DataBits8 = 8			//8 data bits
		};

		enum Parity
		{
			//windows 0-4=None,Odd,Even,Mark,Space
			ParityNone  = 0,			//No Parity
			ParityOdd   = 1,			//Odd Parity
			ParityEven  = 2,			//Even Parity
			ParityMark  = 3,            //Mark Parity
			ParitySpace = 4,			//Space Parity	
		};

		enum StopBits
		{
			//windows 0,1,2 = 1, 1.5, 2
			StopOne = 0,				//1 stop bit
			StopOneAndHalf = 1,			//1.5 stop bit - This is only for the Windows platform
			StopTwo = 2					//2 stop bit
		};

		enum FlowConctrol
		{
			FlowNone = 0,		//No flow control
			FlowHardware = 1,	//Hardware(RTS / CTS) flow control
			FlowSoftware = 2	//Software(XON / XOFF) flow control
		};

		enum SerialPortError
		{
			NoError,							//No error occurred
			DeviceNotFoundError,
			PermissionError,
			OpenError,
			ParityError,
			FramingError,
			BreakConditionError,
			WriteError,							//write error
			ReadError,							//read error
			ResourceError,
			UnsupportedOperationError,
			UnknownError,
			TimeoutError,
			NotOpenError
		};
}

#endif //__CSERIALPORT_GLOBAL_H__