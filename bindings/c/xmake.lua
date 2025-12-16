add_includedirs(".") -- cserialport.h

-- CSerialPort c binding shared library
target("cserialport")
    set_kind("shared")
    add_defines("CSERIALPORT_BINDING_LANGUAGE=C") -- CSerialPort Binding Language
    add_files("$(projectdir)/lib/version.rc")
    add_files("cserialport.cpp")

-- CSerialPort c binding static library
target("cserialport-static")
    add_defines("CSERIALPORT_BINDING_LANGUAGE=C") -- CSerialPort Binding Language
    set_kind("static")
    add_files("cserialport.cpp")

-- CSerialPort c binding example
target("CSerialPortDemoC")
    set_kind("binary")
    if is_config("CSERIALPORT_ENABLE_NATIVE_SYNC", true) then
        add_files("example/main_sync_native.c")
    else
        add_files("example/main.c")
    end
    add_deps("cserialport")
