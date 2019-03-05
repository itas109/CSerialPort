HEADERS += $$PWD/../../src/osplatformutil.h
HEADERS += $$PWD/../../src/SerialPort_global.h
HEADERS += $$PWD/../../src/sigslot.h
HEADERS += $$PWD/../../src/SerialPortBase.h
HEADERS += $$PWD/../../src/SerialPort.h
HEADERS += $$PWD/../../src/SerialPortInfoBase.h
HEADERS += $$PWD/../../src/SerialPortInfo.h

win32 {
    message("Building for Windows")
    HEADERS += $$PWD/../../src/SerialPortInfoWinBase.h
    HEADERS += $$PWD/../../src/SerialPortWinBase.h
}

unix {
    message("Building for unix")
    HEADERS += $$PWD/../../src/SerialPortUnixBase.h
    HEADERS += $$PWD/../../src/SerialPortInfoUnixBase.h
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

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src
