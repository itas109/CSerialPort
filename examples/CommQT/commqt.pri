HEADERS += $$PWD/../../include/CSerialPort/osplatformutil.h
HEADERS += $$PWD/../../include/CSerialPort/SerialPort_global.h
HEADERS += $$PWD/../../include/CSerialPort/sigslot.h
HEADERS += $$PWD/../../include/CSerialPort/SerialPortBase.h
HEADERS += $$PWD/../../include/CSerialPort/SerialPort.h
HEADERS += $$PWD/../../include/CSerialPort/SerialPortInfoBase.h
HEADERS += $$PWD/../../include/CSerialPort/SerialPortInfo.h

win32 {
    message("Building for Windows")
    HEADERS += $$PWD/../../include/CSerialPort/SerialPortInfoWinBase.h
    HEADERS += $$PWD/../../include/CSerialPort/SerialPortWinBase.h
}

unix {
    message("Building for unix")
    HEADERS += $$PWD/../../include/CSerialPort/SerialPortUnixBase.h
    HEADERS += $$PWD/../../include/CSerialPort/SerialPortInfoUnixBase.h
}

SOURCES += $$PWD/../../src/SerialPortBase.cpp
SOURCES += $$PWD/../../src/SerialPort.cpp
SOURCES += $$PWD/../../src/SerialPortInfoBase.cpp
SOURCES += $$PWD/../../src/SerialPortInfo.cpp

win32 {
    SOURCES += $$PWD/../../src/SerialPortWinBase.cpp
    SOURCES += $$PWD/../../src/SerialPortInfoWinBase.cpp
}

unix {
    SOURCES += $$PWD/../../src/SerialPortUnixBase.cpp
    SOURCES += $$PWD/../../src/SerialPortInfoUnixBase.cpp
}

INCLUDEPATH += $$PWD/../../include
DEPENDPATH += $$PWD/../../src
