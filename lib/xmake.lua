-- cserialport shared library
target("libcserialport")
    set_kind("shared")
    add_files("version.rc")

-- cserialport static library
target("libcserialport-static")
    set_kind("static")