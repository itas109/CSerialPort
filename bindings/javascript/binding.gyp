{
    'targets': [
        {
            'target_name': 'cserialport',
            'include_dirs': ['../../include'],
            'sources': [
                '../../src/SerialPortInfo.cpp',
                '../../src/SerialPortInfoBase.cpp',
                'cserialport_wrap.cxx'],
            'conditions': [
                ['OS=="win"', {
                    'sources': ['../../src/SerialPortInfoWinBase.cpp']
                }],
                ['OS=="linux"', {
                    'cflags': ['-fexceptions'],
                    'cflags_cc': ['-fexceptions'],
                    'sources': ['../../src/SerialPortInfoUnixBase.cpp']
                }],
                ['OS=="mac"', {
                    'cflags': ['-fexceptions'],
                    'cflags_cc': ['-fexceptions'],
                    'sources': ['../../src/SerialPortInfoUnixBase.cpp']
                }]
            ]
        }
    ]
}
