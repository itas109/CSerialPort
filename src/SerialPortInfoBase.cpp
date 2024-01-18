#include "CSerialPort/SerialPortInfoBase.h"
#include "CSerialPort/iutils.hpp"
#include <string.h> // strcmp

CSerialPortInfoBase::CSerialPortInfoBase() {}

CSerialPortInfoBase::~CSerialPortInfoBase() {}

bool lessConstString::operator()(const char *lhs, const char *rhs) const
{
    return strcmp(lhs, rhs) < 0;
}

HardwareIdDespSingleton HardwareIdDespSingleton::g_instance;

#define HW_MAP_INSERT(hwId, str) m_hwIdMap.insert(std::make_pair(hwId, str))
HardwareIdDespSingleton::HardwareIdDespSingleton()
{
    // 1a86  QinHeng Electronics
    HW_MAP_INSERT("1a86", "QinHeng");
    HW_MAP_INSERT("1a86:5512", "QinHeng CH341 EPP/I2C adapter");
    HW_MAP_INSERT("1a86:5523", "QinHeng CH341 serial converter");
    HW_MAP_INSERT("1a86:7522", "QinHeng CH340 serial converter");
    HW_MAP_INSERT("1a86:7523", "QinHeng CH340 serial converter");

    // 067b  Prolific Technology, Inc.
    HW_MAP_INSERT("067b", "Prolific");
    HW_MAP_INSERT("067b:0000", "Prolific PL2301 USB-USB Bridge");
    HW_MAP_INSERT("067b:0001", "Prolific PL2302 USB-USB Bridge");
    HW_MAP_INSERT("067b:0307", "Prolific Motorola Serial Adapter");
    HW_MAP_INSERT("067b:04bb", "Prolific PL2303 Serial (IODATA USB-RSAQ2)");
    HW_MAP_INSERT("067b:2303", "Prolific PL2303 Serial Port");
    HW_MAP_INSERT("067b:23a3", "Prolific ATEN Serial Bridge");
    HW_MAP_INSERT("067b:aaa2", "Prolific PL2303 Serial Adapter (IODATA USB-RSAQ3)");
    HW_MAP_INSERT("067b:aaa3", "Prolific PL2303x Serial Adapter");

    // 03eb Atmel Corp.
    HW_MAP_INSERT("03eb", "Atmel");
    HW_MAP_INSERT("03eb:204b", "Atmel LUFA USB to Serial Adapter Project");
    HW_MAP_INSERT("03eb:2068", "Atmel LUFA Virtual Serial/Mass Storage Demo");

    // 0403  Future Technology Devices International, Ltd
    HW_MAP_INSERT("0403", "FTDI");
    HW_MAP_INSERT("0403:0232", "FTDI Serial Converter");
    HW_MAP_INSERT("0403:6001", "FTDI FT232 Serial (UART) IC");
    HW_MAP_INSERT("0403:6007", "FTDI Serial Converter");
    HW_MAP_INSERT("0403:6008", "FTDI Serial Converter");
    HW_MAP_INSERT("0403:6009", "FTDI Serial Converter");
    HW_MAP_INSERT("0403:8372", "FTDI FT8U100AX Serial Port");
    HW_MAP_INSERT("0403:ed71", "FTDI HAMEG HO870 Serial Port");
    HW_MAP_INSERT("0403:ed72", "FTDI HAMEG HO720 Serial Port");
    HW_MAP_INSERT("0403:ed73", "FTDI HAMEG HO730 Serial Port");
    HW_MAP_INSERT("0403:ed74", "FTDI HAMEG HO820 Serial Port");
    HW_MAP_INSERT("0403:f070", "FTDI Serial Converter 422/485 [Vardaan VEUSB422R3]");
    HW_MAP_INSERT("0403:f3c0", "FTDI 4N-GALAXY Serial Converter");
    HW_MAP_INSERT("0403:8372", "FTDI FT8U100AX Serial Port");
    HW_MAP_INSERT("0403:8372", "FTDI FT8U100AX Serial Port");

    // 0483  STMicroelectronics
    HW_MAP_INSERT("0483", "STMicroelectronics");
    HW_MAP_INSERT("0483:0adb", "STMicroelectronics Android Debug Bridge (ADB) device");
    HW_MAP_INSERT("0483:0afb", "STMicroelectronics Android Fastboot device");
    HW_MAP_INSERT("0483:5740", "STMicroelectronics Virtual COM Port");
    HW_MAP_INSERT("0483:7270", "STMicroelectronics ST Micro Serial Bridge");

    // 04b3  IBM
    HW_MAP_INSERT("04b3", "IBM");
    HW_MAP_INSERT("04b3:4482", "IBM Serial Converter");

    // 0557  ATEN International Co., Ltd
    HW_MAP_INSERT("0557", "ATEN");
    HW_MAP_INSERT("0557:2008", "ATEN UC-232A Serial Port [pl2303]");
    HW_MAP_INSERT("0557:2011", "ATEN UC-2324 4xSerial Ports [mos7840]");
    HW_MAP_INSERT("0557:7820", "ATEN UC-2322 2xSerial Ports [mos7820]");

    // 058f  Alcor Micro Corp.
    HW_MAP_INSERT("058f", "Alcor");
    HW_MAP_INSERT("058f:9720", "Alcor USB-Serial Adapter");
};

HardwareIdDespSingleton::~HardwareIdDespSingleton(){};

HardwareIdDespSingleton *HardwareIdDespSingleton::getInstance()
{
    return &g_instance;
}

void HardwareIdDespSingleton::getHardwareIdDescription(const char *hardwareId, char *hardwareDesp)
{
    // 1a86:7523
    if (9 == strlen(hardwareId))
    {
        HardwareIdDespMap::iterator it = m_hwIdMap.find(hardwareId);
        if (it == m_hwIdMap.end())
        {
            int vid = -1;
            if (1 == sscanf(hardwareId, "%04x:", &vid))
            {
                char vidStr[5] = {0};
                itas109::IUtils::strFormat(vidStr, 5, "%04x", vid);

                it = m_hwIdMap.find(vidStr);
                if (it != m_hwIdMap.end())
                {
                    itas109::IUtils::strFormat(hardwareDesp, 256, "%s serial", it->second);
                }
            }
        }
        else
        {
            itas109::IUtils::strncpy(hardwareDesp, it->second, 256);
        }
    }
}
