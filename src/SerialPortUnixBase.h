#ifndef __CSERIALPORTUNIXBASE_H__
#define __CSERIALPORTUNIXBASE_H__

#include "SerialPortBase.h"

// sigslot
// https://sourceforge.net/p/sigslot/bugs/8/
#include "sigslot.h"

//https://blog.csdn.net/u010317005/article/details/52384890

// Linux compile
// g++ SerialPort.cpp SerialPortBase.cpp SerialPortUnixBase.cpp -fPIC -shared -o libss.so
// g++ t.cpp -o tt -L. -lss 


class CSerialPortUnixBase :	public CSerialPortBase
{
public:
	CSerialPortUnixBase();
	CSerialPortUnixBase(const std::string & portName);
	virtual ~CSerialPortUnixBase();

	virtual void construct();

	virtual void init(std::string portName, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);

	virtual bool open();
	virtual void close();

	virtual bool isOpened();

	virtual int readData(char *data, int maxSize);
	virtual int	readLineData(char *data, int maxSize);
	virtual int writeData(const char * data, int maxSize);

	virtual void setDebugModel(bool isDebug);

	virtual void setReadTimeInterval(int msecs);

	virtual int getLastError() const;
	virtual void clearError();

	virtual void setPortName(std::string portName);
	virtual std::string getPortName() const;

	virtual void setBaudRate(int baudRate);
	virtual int getBaudRate() const;

	virtual void setParity(itas109::Parity parity);
	virtual itas109::Parity getParity() const;

	virtual void setDataBits(itas109::DataBits dataBits);
	virtual itas109::DataBits getDataBits() const;

	virtual void setStopBits(itas109::StopBits stopbits);
	virtual itas109::StopBits getStopBits() const;

	virtual void setFlowConctrol(itas109::FlowConctrol flowConctrol);
	virtual itas109::FlowConctrol getFlowConctrol() const;

	virtual void setReadBufferSize(int64 size);
	virtual int64 getReadBufferSize() const;

	virtual void setDtr(bool set = true);
	virtual void setRts(bool set = true);

	std::string getVersion();

private:
	//static unsigned int __stdcall commThreadMonitor(LPVOID pParam);//thread monitor
	//bool startThreadMonitor();
	//bool stopThreadMonitor();

	void lock();
	void unlock();

public:
	static sigslot::signal0<> readReady;//sigslot

private:
	std::string m_portName;
	int m_baudRate;
	itas109::Parity m_parity;
	itas109::DataBits m_dataBits;
	itas109::StopBits m_stopbits;
	itas109::FlowConctrol m_flowConctrol;
	int64 m_readBufferSize;
};
#endif //__CSERIALPORTUNIXBASE_H__
