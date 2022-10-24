/* File : cserialport.i */
%module cserialport

#define DLL_EXPORT

%{
#include "CSerialPort/SerialPortInfo.h"
%}

%include <std_string.i> // std::string
%include <std_vector.i> // std::vector

namespace itas109
{
struct SerialPortInfo
{
    std::string portName;
    std::string description;
};
}

namespace std {
    %template(SerialPortInfoVector) vector<itas109::SerialPortInfo>;
}

%include "CSerialPort/SerialPortInfo.h"