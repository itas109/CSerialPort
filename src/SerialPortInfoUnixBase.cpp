#include "CSerialPort/SerialPortInfoUnixBase.h"
#include "CSerialPort/SerialPort_global.h"
#include "CSerialPort/iutils.hpp"

#ifdef I_OS_LINUX
#include <dirent.h>   //scandir
#include <stdlib.h>   // free
#include <sys/stat.h> //S_ISLNK
#include <unistd.h>   // readlink close

#include <string.h> //basename memset strcmp

#include <fcntl.h>
#include <linux/serial.h> //struct serial_struct
#include <sys/ioctl.h>    //ioctl
#include <termios.h>
#elif defined(I_OS_MAC)
#include <sys/param.h>

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h> // kUSBVendorID kUSBProductID
#include <IOKit/serial/IOSerialKeys.h>
#endif

#include <map>

class lessConstString
{
public:
    bool operator()(const char *lhs, const char *rhs) const
    {
        return strcmp(lhs, rhs) < 0;
    }
};

typedef std::map<const char *, const char *, lessConstString> HardwareIdDespMap;

#define HW_MAP_INSERT(hwId, str) m_hwIdMap.insert(std::make_pair(hwId, str))

class HardwareIdDespSingleton
{
public:
    static HardwareIdDespSingleton *getInstance()
    {
        return &g_instance;
    }

    void getHardwareIdDescription(const char *hardwareId, char *hardwareDesp)
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

private:
    HardwareIdDespSingleton()
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
    ~HardwareIdDespSingleton(){};
    HardwareIdDespSingleton(const HardwareIdDespSingleton &instance);
    HardwareIdDespSingleton &operator=(const HardwareIdDespSingleton &instance);

private:
    static HardwareIdDespSingleton g_instance;
    HardwareIdDespMap m_hwIdMap;
};

HardwareIdDespSingleton HardwareIdDespSingleton::g_instance;

#ifdef I_OS_LINUX
void getDriver(const char *tty, char *driverName)
{
    if (NULL == tty || NULL == driverName)
    {
        return;
    }
    char deviceDir[256] = {0};
    // /sys/class/tty/ttyUSB0/device
    itas109::IUtils::strFormat(deviceDir, 256, "%s/device", tty);

    // Stat the devicedir and handle it if it is a symlink
    struct stat st;
    if (lstat(deviceDir, &st) == 0 && S_ISLNK(st.st_mode))
    {
        // /sys/class/tty/ttyUSB0/device/driver
        char driverDir[256] = {0};
        itas109::IUtils::strFormat(driverDir, 256, "%s/driver", deviceDir);

        char buffer[256] = {0};
        // VCs, ptys, etc don't have /sys/class/tty/<tty>/device/driver
        if (readlink(driverDir, buffer, 256) > 0)
        {
            itas109::IUtils::strncpy(driverName, basename(buffer), 256);
        }
    }
}

void getHardwareId(const char *tty, char *hardwareId)
{
    if (NULL == tty || NULL == hardwareId)
    {
        return;
    }
    char modaliasFile[256] = {0};
    // /sys/class/tty/ttyUSB0/device/modalias
    itas109::IUtils::strFormat(modaliasFile, 256, "%s/device/modalias", tty);

    // get vid and pid
    FILE *fp;
    fp = fopen(modaliasFile, "r");
    if (NULL != fp)
    {
        char buffer[100];
        fread(buffer, 100, 1, fp);
        // usb:v1A86p7523d0264dcFFdsc00dp00icFFisc01ip02in00
        int vid = -1;
        int pid = -1;
        if (2 == sscanf(buffer, "usb:v%04xp%04x", &vid, &pid))
        {
            itas109::IUtils::strFormat(hardwareId, 10, "%04x:%04x", vid, pid);
        }
        fclose(fp);
    }
}

bool isSerial8250Valid(const char *com8250)
{
    bool bRet = false;
    // check /dev/ttyUSB0, not /sys/class/tty/ttyUSB0
    int fd = open(com8250, O_RDWR | O_NONBLOCK | O_NOCTTY);
    if (fd >= 0)
    {
        struct serial_struct serialInfo;
        if (0 == ioctl(fd, TIOCGSERIAL, &serialInfo))
        {
            if (PORT_UNKNOWN != serialInfo.type)
            {
                bRet = true;
            }
        }
        close(fd);
    }

    return bRet;
}

void getTtyPortInfoListLinux(std::vector<const char *> &ttyComList, std::vector<itas109::SerialPortInfo> &portInfoList)
{
    for (size_t i = 0; i < ttyComList.size(); i++)
    {
        const char *ttyDir = ttyComList[i];
        // get driver name
        char driverName[256] = {0};
        getDriver(ttyDir, driverName);

        // only tty device has driver
        if (0 != strcmp(driverName, ""))
        {
            char devName[256] = {0};
            itas109::IUtils::strFormat(devName, 256, "/dev/%s", basename(ttyDir));

            // serial8250 must check
            if (0 == strcmp(driverName, "serial8250") && !isSerial8250Valid(devName))
            {
                continue;
            }

            // get vid and pid
            char hardwareId[256] = {0};
            getHardwareId(ttyDir, hardwareId);

            // get description
            char description[256] = {0};
            HardwareIdDespSingleton::getInstance()->getHardwareIdDescription(hardwareId, description);
            if ('\0' == description[0])
            {
                itas109::IUtils::strncpy(description, driverName, 256);
            }

            itas109::SerialPortInfo serialPortInfo;
            itas109::IUtils::strncpy(serialPortInfo.portName, devName, 256);
            itas109::IUtils::strncpy(serialPortInfo.description, description, 256);
            itas109::IUtils::strncpy(serialPortInfo.hardwareId, hardwareId, 256);
            portInfoList.push_back(serialPortInfo);
        }
    }
}

bool scanDirList(const char *dir, std::vector<const char *> &dirList, std::vector<const char *> filter = std::vector<const char *>())
{
    // https://stackoverflow.com/questions/2530096/how-to-find-all-serial-devices-ttys-ttyusb-on-linux-without-opening-them
    struct dirent **entryList;
    int n = scandir(dir, &entryList, NULL, versionsort); // alphasort versionsort
    if (n >= 0)
    {
        while (n--)
        {
            bool isOK = true;
            for (size_t i = 0; i < filter.size(); ++i)
            {
                if (0 == strcmp(entryList[n]->d_name, filter[i]))
                {
                    isOK = false;
                    break;
                }
            }

            if (isOK)
            {
                char *deviceDir = new char[256]; // delete[]
                itas109::IUtils::strFormat(deviceDir, 256, "%s%s", dir, entryList[n]->d_name);
                dirList.push_back(deviceDir);
            }

            free(entryList[n]);
        }
        free(entryList);

        return true;
    }
    else
    {
        return false;
    }
}

std::vector<itas109::SerialPortInfo> getPortInfoListLinux()
{
    itas109::SerialPortInfo serialPortInfo;
    std::vector<itas109::SerialPortInfo> portInfoList;

    // 1. scan /sys/class/tty - all tty-devices
    const char *ttyDir = "/sys/class/tty/";
    std::vector<const char *> ttyComList;
    std::vector<const char *> ttyFilter = {".", ".."};
    scanDirList(ttyDir, ttyComList, ttyFilter);
    getTtyPortInfoListLinux(ttyComList, portInfoList);
    for (size_t i = 0; i < ttyComList.size(); i++)
    {
        delete[] ttyComList[i];
    }

    // 2. scan /dev/pts/- all pseudo terminal(such as telnet, ssh etc.)
    const char *ptsDir = "/dev/pts/";
    std::vector<const char *> ptsComList;
    std::vector<const char *> ptsFilter = {".", "..", "ptmx"};
    scanDirList(ptsDir, ptsComList, ptsFilter);
    for (size_t i = 0; i < ptsComList.size(); i++)
    {
        itas109::IUtils::strncpy(serialPortInfo.portName, ptsComList[i], 256);
        itas109::IUtils::strncpy(serialPortInfo.description, basename(ptsComList[i]), 256);
        itas109::IUtils::strncpy(serialPortInfo.hardwareId, "pty terminal", 256);
        portInfoList.push_back(serialPortInfo);
        delete[] ptsComList[i];
    }

    return portInfoList;
}

#endif

#ifdef I_OS_MAC
char *getSerialPath(char *dest, io_object_t &serialPort)
{
    CFTypeRef calloutCFString;

    calloutCFString = IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);

    if (calloutCFString)
    {
        CFStringGetCString(static_cast<CFStringRef>(calloutCFString), dest, MAXPATHLEN, kCFStringEncodingUTF8);

        CFRelease(calloutCFString);
    }

    return dest;
}

char *getHardwareId(char *hardwareId, io_registry_entry_t &device)
{
    CFNumberRef vidNumber = (CFNumberRef)IORegistryEntryCreateCFProperty(device, CFSTR(kUSBVendorID), kCFAllocatorDefault, 0);
    CFNumberRef pidNumber = (CFNumberRef)IORegistryEntryCreateCFProperty(device, CFSTR(kUSBProductID), kCFAllocatorDefault, 0);

    if (vidNumber && pidNumber)
    {
        uint32_t vid = 0, pid = 0;
        CFNumberGetValue(vidNumber, kCFNumberIntType, &vid);
        CFNumberGetValue(pidNumber, kCFNumberIntType, &pid);
        CFRelease(vidNumber);
        CFRelease(pidNumber);

        snprintf(hardwareId, MAXPATHLEN, "%04x:%04x", vid, pid);
        return hardwareId;
    }

    if (vidNumber)
    {
        CFRelease(vidNumber);
    }
    if (pidNumber)
    {
        CFRelease(pidNumber);
    }

    // parent device
    io_registry_entry_t parent = 0;
    kern_return_t result = IORegistryEntryGetParentEntry(device, kIOServicePlane, &parent);
    if (KERN_SUCCESS == result)
    {
        getHardwareId(hardwareId, parent);
        IOObjectRelease(parent);
        return hardwareId;
    }

    // not found
    return hardwareId;
}

std::vector<itas109::SerialPortInfo> getPortInfoListMac()
{
    // https://developer.apple.com/documentation/iokit/communicating_with_a_modem_on_a_serial_port
    itas109::SerialPortInfo m_serialPortInfo;
    std::vector<itas109::SerialPortInfo> portInfoList;

    kern_return_t kernResult;
    CFMutableDictionaryRef classesToMatch;
    io_iterator_t serialPortIterator;

    io_object_t serialPort;

    classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);

    if (classesToMatch == NULL)
        return portInfoList;

    CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDAllTypes));

    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, &serialPortIterator);

    if (KERN_SUCCESS != kernResult)
    {
        return portInfoList;
    }

    while (serialPort = IOIteratorNext(serialPortIterator))
    {
        char device_path[MAXPATHLEN] = {0};
        char device_hardware_id[MAXPATHLEN] = {0};
        getSerialPath(device_path, serialPort);
        getHardwareId(device_hardware_id, serialPort);

        IOObjectRelease(serialPort);

        if ('\0' == device_path[0])
        {
            continue;
        }

        // get description
        char description[MAXPATHLEN] = {0};
        HardwareIdDespSingleton::getInstance()->getHardwareIdDescription(device_hardware_id, description);

        itas109::IUtils::strncpy(m_serialPortInfo.portName, device_path, MAXPATHLEN);
        itas109::IUtils::strncpy(m_serialPortInfo.description, description, MAXPATHLEN);
        itas109::IUtils::strncpy(m_serialPortInfo.hardwareId, device_hardware_id, MAXPATHLEN);
        portInfoList.push_back(m_serialPortInfo);
    }

    IOObjectRelease(serialPortIterator);
    return portInfoList;
}
#endif

std::vector<itas109::SerialPortInfo> getPortInfoList()
{
    std::vector<itas109::SerialPortInfo> portInfoList;
#ifdef I_OS_LINUX
    portInfoList = getPortInfoListLinux();
#elif defined I_OS_MAC
    // ls /dev/{tty,cu}.*
    portInfoList = getPortInfoListMac();
#endif

    return portInfoList;
}

CSerialPortInfoUnixBase::CSerialPortInfoUnixBase() {}

CSerialPortInfoUnixBase::~CSerialPortInfoUnixBase() {}

std::vector<itas109::SerialPortInfo> CSerialPortInfoUnixBase::availablePortInfos()
{
    return getPortInfoList();
}
