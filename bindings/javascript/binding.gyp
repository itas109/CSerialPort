{
    'targets': [
        {
            'target_name': 'cserialport',
            'include_dirs': ['../../include'],
            'sources': [
                '../../src/SerialPort.cpp',
                '../../src/SerialPortBase.cpp',
                '../../src/SerialPortInfo.cpp',
                '../../src/SerialPortInfoBase.cpp',
                'cserialport_wrap.cxx'],
            'conditions': [
                ['OS=="win"', {
                    'sources': [
                        '../../src/SerialPortWinBase.cpp',
                        '../../src/SerialPortInfoWinBase.cpp',
                        ]
                }],
                ['OS=="linux"', {
                    'cflags': ['-fexceptions'],
                    'cflags_cc': ['-fexceptions'],
                    'sources': [
                        '../../src/SerialPortUnixBase.cpp',
                        '../../src/SerialPortInfoUnixBase.cpp'
                        ]
                }],
                ['OS=="mac"', {
                    'sources': [
		    	'../../src/SerialPortUnixBase.cpp',
		    	'../../src/SerialPortInfoUnixBase.cpp'
		    ]
                }]
            ]
        }
    ]
}
