set(CSerialPortRootPath "${CMAKE_CURRENT_LIST_DIR}/..")

include_directories(${CSerialPortRootPath}/include)

list(APPEND CSerialPortHeaderFiles 
  ${CSerialPortRootPath}/include/CSerialPort/SerialPort_global.h
  ${CSerialPortRootPath}/include/CSerialPort/SerialPort_version.h
  ${CSerialPortRootPath}/include/CSerialPort/SerialPort.h
  ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfo.h
  ${CSerialPortRootPath}/include/CSerialPort/SerialPortBase.h
  ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoBase.h
  ${CSerialPortRootPath}/include/CSerialPort/SerialPortListener.h
  ${CSerialPortRootPath}/include/CSerialPort/IProtocolParser.h
  ${CSerialPortRootPath}/include/CSerialPort/ibuffer.hpp
  ${CSerialPortRootPath}/include/CSerialPort/ilog.hpp
  ${CSerialPortRootPath}/include/CSerialPort/ithread.hpp
  ${CSerialPortRootPath}/include/CSerialPort/itimer.hpp
  ${CSerialPortRootPath}/include/CSerialPort/iutils.hpp
)

list(APPEND CSerialPortSourceFiles 
  ${CSerialPortRootPath}/src/SerialPort.cpp
  ${CSerialPortRootPath}/src/SerialPortBase.cpp
  ${CSerialPortRootPath}/src/SerialPortInfo.cpp
  ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp
)
if (CSERIALPORT_ENABLE_NATIVE_SYNC)
  if (WIN32)
      list(APPEND CSerialPortHeaderFiles
        ${CSerialPortRootPath}/include/CSerialPort/SerialPortNativeSyncWinBase.h
        ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoWinBase.h
      )
      list(APPEND CSerialPortSourceFiles
        ${CSerialPortRootPath}/src/SerialPortNativeSyncWinBase.cpp
        ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp
      )
  elseif (UNIX)
      list(APPEND CSerialPortHeaderFiles
        ${CSerialPortRootPath}/include/CSerialPort/SerialPortNativeSyncUnixBase.h
        ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoUnixBase.h
      )
      list(APPEND CSerialPortSourceFiles
        ${CSerialPortRootPath}/src/SerialPortNativeSyncUnixBase.cpp
        ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp
      )
  endif ()
else()
  list(APPEND CSerialPortHeaderFiles${CSerialPortRootPath}/include/CSerialPort/SerialPortAsyncBase.h)
  list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortAsyncBase.cpp)
  if (WIN32)
      list(APPEND CSerialPortHeaderFiles
        ${CSerialPortRootPath}/include/CSerialPort/SerialPortWinBase.h
        ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoWinBase.h
      )
      list(APPEND CSerialPortSourceFiles
        ${CSerialPortRootPath}/src/SerialPortWinBase.cpp
        ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp
      )
  elseif (UNIX)
      list(APPEND CSerialPortHeaderFiles
        ${CSerialPortRootPath}/include/CSerialPort/SerialPortUnixBase.h
        ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoUnixBase.h
      )
      list(APPEND CSerialPortSourceFiles
        ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp
        ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp
      )
  endif ()
endif ()