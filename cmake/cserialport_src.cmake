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
  list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortAsyncBase.h)
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

# CSerialPort target link libraries function
function(cserialport_target_link_libraries TARGET_NAME)
    if (WIN32)
        # for function availableFriendlyPorts
        target_link_libraries( ${TARGET_NAME} user32 advapi32 setupapi)
    elseif(APPLE)
        find_library(IOKIT_LIBRARY IOKit)
        find_library(FOUNDATION_LIBRARY Foundation)
        if (IOKIT_LIBRARY AND FOUNDATION_LIBRARY)
            target_link_libraries( ${TARGET_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
        else()
            message(WARNING "could not find IOKIT_LIBRARY or FOUNDATION_LIBRARY")
        endif()
    elseif(UNIX AND NOT APPLE)
        target_link_libraries( ${TARGET_NAME} pthread)
    else()
        message(WARNING "unsupported platform for ${TARGET_NAME}")
    endif()
endfunction()
