mod bindings {
    include!("../../bindings.rs");
}

use bindings::*;

use std::ffi::{CStr, CString};
use std::os::raw::{c_int, c_void};
use std::ptr;

#[link(name = "cserialport")]
extern "C" {}

static mut COUNT_READ: u32 = 0;

unsafe extern "C" fn on_read_event(
    serial_port: *mut ::std::os::raw::c_void,
    port_name: *const ::std::os::raw::c_char,
    read_buffer_len: ::std::os::raw::c_uint,
) {
    if read_buffer_len > 0 {
        let data = vec![0u8; read_buffer_len as usize + 1]; // '\0'
        let data_ptr = data.as_ptr() as *mut c_void;

        // read
        let rec_len = CSerialPortReadData(serial_port, data_ptr, read_buffer_len as c_int);

        if rec_len > 0 {
            let data_str = String::from_utf8_lossy(&data[..rec_len as usize]);

            let hex_str = data
                .iter()
                .map(|&b| format!("0x{:02X} ", b))
                .collect::<String>();

            let port_name_str = CStr::from_ptr(port_name).to_str().unwrap();

            COUNT_READ += 1;
            println!(
                "{} - Count: {} Length: {}, Str: {}, Hex: {}",
                port_name_str, COUNT_READ, rec_len, data_str, hex_str
            );

            // return receive data
            CSerialPortWriteData(
                serial_port,
                data.as_ptr() as *const c_void,
                rec_len as c_int,
            );
        }
    }
}

unsafe extern "C" fn hot_plug_event(
    serial_port: *mut ::std::os::raw::c_void,
    port_name: *const ::std::os::raw::c_char,
    is_add: ::std::os::raw::c_int,
) {
    let port_name_str = CStr::from_ptr(port_name).to_str().unwrap();

    println!("portName: {}, isAdded: {}", port_name_str, is_add);
}

fn main() {
    unsafe {
        let serial_port = CSerialPortMalloc();

        let version = CStr::from_ptr(CSerialPortGetVersion(serial_port));
        println!("Version: {}", version.to_str().unwrap());

        let mut port_info_array = SerialPortInfoArray {
            size: 0,
            portInfo: ptr::null_mut(),
        };
        CSerialPortAvailablePortInfosMalloc(&mut port_info_array);

        // connect for read
        CSerialPortConnectReadEvent(serial_port, Some(on_read_event));
        // connect for hot plug
        CSerialPortConnectHotPlugEvent(serial_port, Some(hot_plug_event));

        if 0 == port_info_array.size {
            println!("No Valid Port");
        } else {
            println!("Available Friendly Ports:");
            for i in 0..port_info_array.size {
                let port_info = *port_info_array.portInfo.add(i as usize);

                let port_name = CStr::from_ptr(port_info.portName.as_ptr());
                let description = CStr::from_ptr(port_info.description.as_ptr());

                let port_name_str = port_name.to_str().unwrap();
                let description_str = description.to_str().unwrap();

                println!("{} - {} {}", i + 1, port_name_str, description_str);
            }

            let mut input: usize;
            loop {
                println!(
                    "Please input the index of port (1 - {}):",
                    port_info_array.size
                );

                let mut inputStr = String::new();
                std::io::stdin().read_line(&mut inputStr).unwrap();
                input = inputStr.trim().parse().unwrap_or(0);

                if (input >= 1 && input <= port_info_array.size as usize) {
                    break;
                }
            }

            let port_name =
                CStr::from_ptr((*port_info_array.portInfo.add(input - 1)).portName.as_ptr());
            let port_name_str = port_name.to_str().unwrap();
            println!("Port Name: {}", port_name_str);

            CSerialPortInit(
                serial_port,
                port_name.as_ptr(),   // windows:COM1 Linux:/dev/ttyS0
                9600,                 // baudrate
                Parity_ParityNone,    // parity
                DataBits_DataBits8,   // data bit
                StopBits_StopOne,     // stop bit
                FlowControl_FlowNone, // flow
                4096,                 // read buffer size
            );
            CSerialPortSetReadIntervalTimeout(serial_port, 0); // read interval timeout

            CSerialPortOpen(serial_port);

            if 1 == CSerialPortIsOpen(serial_port) {
                println!("Open {} Success", port_name_str);
            } else {
                println!("Open {} Failed", port_name_str);
            }
            let code = CSerialPortGetLastError(serial_port);
            let message = CStr::from_ptr(CSerialPortGetLastErrorMsg(serial_port));
            let message_str = message.to_str().unwrap();
            println!("Code: {}, Message: {}", code, message_str);

            // write hex data
            let hex: [u8; 5] = [0x31, 0x32, 0x33, 0x34, 0x35];
            CSerialPortWriteData(serial_port, hex.as_ptr() as *const c_void, 5);

            // write str data
            let data = CString::new("itas109").unwrap();
            CSerialPortWriteData(serial_port, data.as_ptr() as *const c_void, 7);
        }

        loop {}

        // CSerialPortAvailablePortInfosFree(&mut port_info_array);
        // CSerialPortFree(serial_port);
    }
}
