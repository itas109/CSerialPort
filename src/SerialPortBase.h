#ifndef __CSERIALPORTBASE_H__
#define __CSERIALPORTBASE_H__

#include "SerialPort_global.h"
#include <string>

class CSerialPortBase
{
public:
	CSerialPortBase();
	CSerialPortBase(const std::string & portName);
	virtual ~CSerialPortBase();

	virtual void construct() = 0;

	virtual void init(std::string portName, int baudRate, itas109::Parity parity, itas109::DataBits dataBits, itas109::StopBits stopbits, itas109::FlowConctrol flowConctrol, int64 readBufferSize) = 0;

	virtual void init(int port, int baudRate, itas109::Parity parity, itas109::DataBits dataBits, itas109::StopBits stopbits, itas109::FlowConctrol flowConctrol, int64 readBufferSize) = 0;

	virtual void setOperateMode(itas109::OperateMode operateMode);

	virtual bool open() = 0;
	virtual void close() = 0;

	virtual bool isOpened() = 0;

	virtual int readData(char *data, int maxSize) = 0;
	virtual int readAllData(char *data) = 0;
	virtual int	readLineData(char *data, int maxSize) = 0;
	virtual int writeData(const char * data, int maxSize) = 0;

	virtual void setDebugModel(bool isDebug) = 0;

	virtual void setReadTimeInterval(int msecs) = 0;

	virtual int getLastError() const;
	virtual void clearError();
	
	virtual void setPortName(std::string portName) = 0;
	virtual std::string getPortName() const = 0;

	virtual void setBaudRate(int baudRate) = 0;
	virtual int getBaudRate() const = 0;

	virtual void setParity(itas109::Parity parity) = 0;
	virtual itas109::Parity getParity() const = 0;

	virtual void setDataBits(itas109::DataBits dataBits) = 0;
	virtual itas109::DataBits getDataBits() const = 0;

	virtual void setStopBits(itas109::StopBits stopbits) = 0;
	virtual itas109::StopBits getStopBits() const = 0;

	virtual void setFlowConctrol(itas109::FlowConctrol flowConctrol) = 0;
	virtual itas109::FlowConctrol getFlowConctrol() const = 0;

	virtual void setReadBufferSize(int64 size) = 0;
	virtual int64 getReadBufferSize() const = 0;

	virtual void setDtr(bool set = true) = 0;
	virtual void setRts(bool set = true) = 0;

	std::string getVersion();

protected:
	void lock();
	void unlock();

protected:
	int lastError;
	itas109::OperateMode m_operateMode;
private:

};
#endif //__CSERIALPORTBASE_H__
