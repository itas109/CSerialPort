add_includedirs(".") -- cseiralport_test.h doctest.h

target("CSerialPortTest")
    set_kind("binary")
    add_files("cseiralport_test.cpp","CSerialPortVirtual.cpp")
    if not is_plat("windows") then
        add_syslinks("util") -- pty
    end