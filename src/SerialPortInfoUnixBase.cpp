#include "SerialPortInfoUnixBase.h"

//#include <stdlib.h>
#include <dirent.h>   //scandir
//#include <stdio.h>
//#include <sys/types.h>
#include <sys/stat.h>   //S_ISLNK
#include <unistd.h>     // readlink close

#include <string.h>     //basename memset strcmp

#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>      //ioctl
#include <linux/serial.h> //struct serial_struct

//#include <iostream>

CSerialPortInfoUnixBase::CSerialPortInfoUnixBase()
{
}


CSerialPortInfoUnixBase::~CSerialPortInfoUnixBase()
{
}

std::list<std::string> CSerialPortInfoUnixBase::availablePorts()
{
    // ttyS*    // Standard UART 8250 and etc
    // ttyO*    // OMAP UART 8250 and etc
    // ttyUSB*  // Usb/serial converters PL2303 and etc
    // ttyACM*  // CDC_ACM converters (i.e. Mobile Phones)
    // ttyGS*   // Gadget serial device (i.e. Mobile Phones with gadget serial driver)
    // ttyMI*   // MOXA pci/serial converters
    // ttymxc*  // Motorola IMX serial ports (i.e. Freescale i.MX)
    // ttyAMA*  // AMBA serial device for embedded platform on ARM (i.e. Raspberry Pi)
    // ttyTHS*  // Serial device for embedded platform on ARM (i.e. Tegra Jetson TK1)
    // rfcomm*  // Bluetooth serial device
    // ircomm*  // IrDA serial device
    // tnt*     // Virtual tty0tty serial device
    // pts/*    // Virtual pty serial device

    //need add virtual serial port support 19-07-28

    return getComList();
}

std::list<std::string> CSerialPortInfoUnixBase::availableFriendlyPorts()
{
    // Not Support,so it is equal to availablePorts()
    return availablePorts();
}

static std::string get_driver(const std::string& tty)
{
    struct stat st;
    std::string devicedir = tty;

    // Append '/device' to the tty-path
    ///sys/class/tty/ttyS0/device
    devicedir += "/device";

    // Stat the devicedir and handle it if it is a symlink
    if (lstat(devicedir.c_str(), &st)==0 && S_ISLNK(st.st_mode))
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Append '/driver' and return basename of the target
        // VCs, ptys, etc don't have /sys/class/tty/<tty>/device/driver's
        // /sys/class/tty/ttyS0/device/driver
        devicedir += "/driver";

        if (readlink(devicedir.c_str(), buffer, sizeof(buffer)) > 0)
        {
            //std::cout << "readlink " << devicedir << ", baseName " << basename(buffer) << std::endl;

            return basename(buffer);
        }
        else
        {
            //std::cout << "readlink error " << devicedir << std::endl;
        }
    }
    else
    {
        //std::cout << "lstat error " << devicedir << std::endl;
    }
    return "";
}

static void register_comport( std::list<std::string>& comList, std::list<std::string>& comList8250, const std::string& dir) {
    // Get the driver the device is using
    std::string driver = get_driver(dir);

    // Skip devices without a driver
    if (driver.size() > 0) {
        std::string devfile = std::string("/dev/") + basename(dir.c_str());

        //std::cout << "driver : " << driver << ", devfile : " << devfile << std::endl;

        // Put serial8250-devices in a seperate list
        if (driver == "serial8250") {
            comList8250.push_back(devfile);
        }
        else
        {
            comList.push_back(devfile);
        }
    }
}

static void probe_serial8250_comports(std::list<std::string>& comList, std::list<std::string> comList8250) {
    struct serial_struct serinfo;
    std::list<std::string>::iterator it = comList8250.begin();

    // Iterate over all serial8250-devices
    while (it != comList8250.end()) {

        // Try to open the device
        int fd = open((*it).c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY);

        if (fd >= 0) {
            // Get serial_info
            if (ioctl(fd, TIOCGSERIAL, &serinfo)==0) {
                // If device type is no PORT_UNKNOWN we accept the port
                if (serinfo.type != PORT_UNKNOWN)
                {
                    comList.push_back(*it);
                    //std::cout << "+++  " << *it << std::endl;
                }
                else
                {
                    //std::cout << "PORT_UNKNOWN " << *it << std::endl;
                }
            }
            close(fd);
        }
        it ++;
    }
}

std::list<std::string> CSerialPortInfoUnixBase::getComList()
{
    int n;
    struct dirent **namelist;
    std::list<std::string> comList;
    std::list<std::string> comList8250;
    const char* sysdir = "/sys/class/tty/";

    // Scan through /sys/class/tty - it contains all tty-devices in the system
    n = scandir(sysdir, &namelist, NULL, NULL);
    if (n < 0)
    {
        perror("scandir");
    }
    else
    {
        while (n--)
        {
            if (strcmp(namelist[n]->d_name,"..") && strcmp(namelist[n]->d_name,"."))
            {
                // Construct full absolute file path
                std::string devicedir = sysdir;
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

    // Return the lsit of detected comports
    return comList;
}
