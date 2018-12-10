#ifndef __CSERIALPORT_H__
#define __CSERIALPORT_H__

#include "SerialPort_global.h"
#include <string>

#include "sigslot.h"
using namespace sigslot;

class CSerialPortBase;

namespace itas109
{
	class DLL_EXPORT CSerialPort : public has_slots<>
	{
	public:
		CSerialPort();
		CSerialPort(const std::string & portName);
		~CSerialPort();

		void init(std::string portName, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);
		void init(int port, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);

		bool open();
		void close();

		bool isOpened();

		int readData(char *data, int maxSize);
		int readAllData(char *data);
		int	readLineData(char *data, int maxSize);
		int writeData(const char * data, int maxSize);

		void setDebugModel(bool isDebug);

		void setReadTimeInterval(int msecs);

		 int getLastError() const;
		 void clearError();

		 void setPortName(std::string portName);
		 std::string getPortName() const;

		 void setBaudRate(int baudRate);
		 int getBaudRate() const;

		 void setParity(itas109::Parity parity);
		 itas109::Parity getParity() const;

		 void setDataBits(itas109::DataBits dataBits);
		 itas109::DataBits getDataBits() const;

		void setStopBits(itas109::StopBits stopbits);
		itas109::StopBits getStopBits() const;

		void setFlowConctrol(itas109::FlowConctrol flowConctrol);
		itas109::FlowConctrol getFlowConctrol() const;

		void setReadBufferSize(int64 size);
		int64 getReadBufferSize() const;

		void setDtr(bool set = true);
		void setRts(bool set = true);

		std::string getVersion();

	public:
		void onReadReady();
		sigslot::signal0<> readReady;//sigslot

	private:
		CSerialPortBase * p_serialPortBase;
	};
}
#endif //__CSERIALPORT_H__