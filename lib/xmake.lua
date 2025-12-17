-- cserialport shared library
target("libcserialport")
    set_kind("shared")
    if is_plat("windows") then
        add_files("version.rc")
    end

-- cserialport static library
target("libcserialport-static")
    set_kind("static")