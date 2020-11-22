#! /usr/bin/env python
#coding=utf-8

import pty
import os
import select

def mkpty():
    # open pseudo terminal
    master1, slave = pty.openpty()
    slaveName1 = os.ttyname(slave)
    master2, slave = pty.openpty()
    slaveName2 = os.ttyname(slave)
    print '\nslave device names: ', slaveName1, slaveName2
    return master1, master2

if __name__ == "__main__":
    master1, master2 = mkpty()
    while True:
        rl, wl, el = select.select([master1,master2], [], [], 1)
        for master in rl:
            data = os.read(master, 128)
            print "read %d data." % len(data)
            if master==master1:
                os.write(master2, data)
            else:
                os.write(master1, data)