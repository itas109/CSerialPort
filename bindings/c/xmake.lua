local kind = get_config("kind") or "static"

-- CSerialPort c binding shared library
target("cserialport")
    set_kind(kind)
    add_defines("CSERIALPORT_BINDING_LANGUAGE=C") -- CSerialPort Binding Language
    if is_plat("windows") and kind == "shared" then
        add_files("$(projectdir)/lib/version.rc")
    end
    add_includedirs(".", {public = true}) -- cserialport.h
    add_headerfiles("cserialport.h", {prefixdir = ""})
    add_files("cserialport.cpp")

-- CSerialPort c binding example
if is_config("CSERIALPORT_BUILD_EXAMPLES", true) then
    target("CSerialPortDemoC")
        set_kind("binary")
        if is_config("CSERIALPORT_ENABLE_NATIVE_SYNC", true) then
            add_files("example/main_sync_native.c")
        else
            add_files("example/main.c")
        end
        add_deps("cserialport")
end