add_rules("mode.debug", "mode.release")

set_languages("c++11")

option("CSERIALPORT_ENABLE_NATIVE_SYNC", {default = false, description = "Enable CSerialPort native synchronous mode"})
option("CSERIALPORT_BUILD_EXAMPLES", {default = true, description = "Enable CSerialPort examples"})
option("CSERIALPORT_BUILD_BINDING_C", {default = true, description = "Enable CSerialPort c binding"})
option("CSERIALPORT_ENABLE_DEBUG", {default = false, description = "Enable CSerialPort debug log"})
option("CSERIALPORT_ENABLE_UTF8", {default = false, description = "Enable CSerialPort UTF8"})
option("CSERIALPORT_BUILD_DOC", {default = false, description = "Enable CSerialPort doc"})
option("CSERIALPORT_BUILD_TEST", {default = true, description = "Enable CSerialPort test"})

if is_config("CSERIALPORT_ENABLE_NATIVE_SYNC", true) then
    add_defines("CSERIALPORT_NATIVE_SYNC") -- CSerialPort Native Sync Mode
end

if is_config("CSERIALPORT_ENABLE_DEBUG", true) then
    add_defines("CSERIALPORT_DEBUG") -- CSerialPort Debug Log
end

if is_config("CSERIALPORT_ENABLE_UTF8", true) then
    add_defines("CSERIALPORT_UTF8") -- CSerialPort UTF8
end

-- common include dirs
add_includedirs("include")

-- common source files
add_files("src/SerialPort.cpp", "src/SerialPortBase.cpp", "src/SerialPortInfo.cpp", "src/SerialPortInfoBase.cpp")
if (CSERIALPORT_ENABLE_NATIVE_SYNC) then
    if is_plat("windows") then
        add_files("src/SerialPortNativeSyncWinBase.cpp", "src/SerialPortInfoWinBase.cpp")
    else
        add_files("src/SerialPortNativeSyncUnixBase.cpp", "src/SerialPortInfoUnixBase.cpp")
    end
else
    add_files("src/SerialPortAsyncBase.cpp")
    if is_plat("windows") then
        add_files("src/SerialPortWinBase.cpp", "src/SerialPortInfoWinBase.cpp")
    else
        add_files("src/SerialPortUnixBase.cpp", "src/SerialPortInfoUnixBase.cpp")
    end
end

-- common system links
if is_plat("windows") then
    add_syslinks("user32", "advapi32", "setupapi")
elseif is_plat("linux") then
    add_syslinks("pthread")
elseif is_plat("macosx") then
    add_syslinks("Foundation", "IOKit")
end

-- libcserialport
includes("lib")

-- examples
if is_config("CSERIALPORT_BUILD_EXAMPLES", true) then
    includes("examples")
end

-- bindings/c
if is_config("CSERIALPORT_BUILD_BINDING_C", true) then
    includes("bindings/c")
end

-- test
if is_config("CSERIALPORT_BUILD_TEST", true) then
    includes("test")
end

-- doc
if is_config("CSERIALPORT_BUILD_DOC", true) then
    -- includes("doc")
end
