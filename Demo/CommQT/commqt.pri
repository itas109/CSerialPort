HEADERS += $$PWD/../../src/osplatformutil.h
HEADERS += $$PWD/../../src/SerialPort_global.h
HEADERS += $$PWD/../../src/sigslot.h
HEADERS += $$PWD/../../src/SerialPortBase.h
HEADERS += $$PWD/../../src/SerialPort.h
HEADERS += $$PWD/../../src/SerialPortInfoBase.h
HEADERS += $$PWD/../../src/SerialPortInfo.h

win32:HEADERS += $$PWD/../../src/SerialPortWinBase.h
#else:unix:HEADERS += $$PWD/../../src/SerialPortUnixBase.h
win32:HEADERS += $$PWD/../../src/SerialPortInfoWinBase.h
#else:unix:HEADERS += $$PWD/../../src/SerialPortInfoUnixBase.h

SOURCES += $$PWD/../../src/SerialPortBase.cpp
SOURCES += $$PWD/../../src/SerialPort.cpp
SOURCES += $$PWD/../../src/SerialPortInfoBase.cpp
SOURCES += $$PWD/../../src/SerialPortInfo.cpp

win32:SOURCES += $$PWD/../../src/SerialPortWinBase.cpp
##else:unix:SOURCES += $$PWD/../../src/SerialPortUnixBase.cpp
win32:SOURCES += $$PWD/../../src/SerialPortInfoWinBase.cpp
#else:unix:SOURCES += $$PWD/../../src/SerialPortInfoUnixBase.cpp

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src
