target("CSerialPortDemoNoGuiProtocol")
    add_defines("CSERIALPORT_DEBUG")
    set_kind("binary")
    add_files("CommNoGuiProtocol.cpp")