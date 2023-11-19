#include "CSerialPort/SerialPortInfoUnixBase.h"
#include "CSerialPort/SerialPort_global.h"
#include "CSerialPort/iutils.hpp"

#ifdef I_OS_LINUX
#include <dirent.h>   //scandir
#include <stdlib.h>   // free
#include <sys/stat.h> //S_ISLNK
#include <unistd.h>   // readlink close

#include <string.h> //basename memset strcmp
#include <string>   // std::string

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

#ifdef I_OS_LINUX
std::string get_driver(const std::string &tty)
{
    struct stat st;
    std::string devicedir = tty;

    // Append '/device' to the tty-path
    /// sys/class/tty/ttyS0/device
    devicedir += "/device";

    // Stat the devicedir and handle it if it is a symlink
    if (lstat(devicedir.c_str(), &st) == 0 && S_ISLNK(st.st_mode))
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Append '/driver' and return basename of the target
        // VCs, ptys, etc don't have /sys/class/tty/<tty>/device/driver's
        // /sys/class/tty/ttyS0/device/driver
        devicedir += "/driver";

        if (readlink(devicedir.c_str(), buffer, sizeof(buffer)) > 0)
        {
            // std::cout << "readlink " << devicedir << ", baseName " << basename(buffer) << std::endl;

            return basename(buffer);
        }
        else
        {
            // std::cout << "readlink error " << devicedir << std::endl;
        }
    }
    else
    {
        // std::cout << "lstat error " << devicedir << std::endl;
    }
    return "";
}

void register_comport(std::vector<std::string> &comList, std::vector<std::string> &comList8250, const std::string &dir)
{
    // Get the driver the device is using
    std::string driver = get_driver(dir);

    // Skip devices without a driver
    if (driver.size() > 0)
    {
        std::string devfile = std::string("/dev/") + basename(dir.c_str());

        // std::cout << "driver : " << driver << ", devfile : " << devfile << std::endl;

        // Put serial8250-devices in a seperate list
        if (driver == "serial8250")
        {
            comList8250.push_back(devfile);
        }
        else
        {
            comList.push_back(devfile);
        }
    }
}

void probe_serial8250_comports(std::vector<std::string> &comList, std::vector<std::string> comList8250)
{
    struct serial_struct serinfo;
    std::vector<std::string>::iterator it = comList8250.begin();

    // Iterate over all serial8250-devices
    while (it != comList8250.end())
    {

        // Try to open the device
        int fd = open((*it).c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY);

        if (fd >= 0)
        {
            // Get serial_info
            if (ioctl(fd, TIOCGSERIAL, &serinfo) == 0)
            {
                // If device type is no PORT_UNKNOWN we accept the port
                if (serinfo.type != PORT_UNKNOWN)
                {
                    comList.push_back(*it);
                    // std::cout << "+++  " << *it << std::endl;
                }
                else
                {
                    // std::cout << "PORT_UNKNOWN " << *it << std::endl;
                }
            }
            close(fd);
        }
        ++it;
    }
}

std::vector<std::string> getPortInfoListLinux()
{
    // https://stackoverflow.com/questions/2530096/how-to-find-all-serial-devices-ttys-ttyusb-on-linux-without-opening-them
    int n = -1;
    struct dirent **namelist;
    std::vector<std::string> comList;
    std::vector<std::string> comList8250;
    const char *sysDir = "/sys/class/tty/";
    const char *ptsDir = "/dev/pts/";

    // 1.Scan through /sys/class/tty - it contains all tty-devices in the system
    n = scandir(sysDir, &namelist, NULL, NULL);
    if (n >= 0)
    {
        while (n--)
        {
            if (strcmp(namelist[n]->d_name, "..") && strcmp(namelist[n]->d_name, "."))
            {
                // Construct full absolute file path
                std::string devicedir = sysDir;
                devicedir += namelist[n]->d_name;

                // Register the device
                register_comport(comList, comList8250, devicedir);
            }
            free(namelist[n]);
        }
        free(namelist);
    }

    // Only non-serial8250 has been added to comList without any further testing
    // serial8250-devices must be probe to check for validity
    probe_serial8250_comports(comList, comList8250);

    // 2.Scan through /dev/pts/- it contains all pseudo terminal(such as telnet, ssh etc.) in the system
    n = scandir(ptsDir, &namelist, NULL, NULL);
    if (n >= 0)
    {
        while (n--)
        {
            if (strcmp(namelist[n]->d_name, "..") && strcmp(namelist[n]->d_name, ".") && strcmp(namelist[n]->d_name, "ptmx"))
            {
                // Construct full absolute file path
                std::string ptsName = ptsDir;
                ptsName += namelist[n]->d_name;

                comList.push_back(ptsName);
            }
            free(namelist[n]);
        }
        free(namelist);
    }

    // Return the lsit of detected comports
    return comList;
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

        snprintf(hardwareId, MAXPATHLEN, "VID_%04X&PID_%04X", vid, pid);
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

        itas109::IUtils::strncpy(m_serialPortInfo.portName, device_path, MAXPATHLEN);
        itas109::IUtils::strncpy(m_serialPortInfo.description, "", 1);
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
    // TODO: need to optimize
    itas109::SerialPortInfo m_serialPortInfo;
    std::vector<std::string> portList = getPortInfoListLinux();

    int count = portList.size();

    for (int i = 0; i < count; i++)
    {
        itas109::IUtils::strncpy(m_serialPortInfo.portName, portList[i].c_str(), 256);
        itas109::IUtils::strncpy(m_serialPortInfo.description, "", 1);
        itas109::IUtils::strncpy(m_serialPortInfo.hardwareId, "", 1);
        portInfoList.push_back(m_serialPortInfo);
    }
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
