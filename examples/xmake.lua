-- CommNoGui
includes("CommNoGui")

if is_config("CSERIALPORT_ENABLE_NATIVE_SYNC", false) then
    -- CommNoGuiProtocol
    includes("CommNoGuiProtocol")
end