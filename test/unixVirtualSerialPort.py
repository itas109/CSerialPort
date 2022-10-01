#! /usr/bin/env python
#coding=utf-8

from __future__ import print_function # python2 use print() function

import pty
import os
import select

def mkpty():
    # open pseudo terminal
    master1, slave = pty.openpty()
    slaveName1 = os.ttyname(slave)
    master2, slave = pty.openpty()
    slaveName2 = os.ttyname(slave)
    print('virtual serial port names: ', slaveName1, slaveName2)
    return master1, master2, slaveName1, slaveName2

if __name__ == "__main__":
    master1, master2, slaveName1, slaveName2 = mkpty()
    while True:
        rl, wl, el = select.select([master1,master2], [], [], 1)
        for master in rl:
            data = os.read(master, 1024)
            if master==master1:
                print('from %s - [%d] %s' %(slaveName1, len(data), data))
                os.write(master2, data)
            else:
                print('from %s - [%d] %s' %(slaveName2, len(data), data))
                os.write(master1, data)