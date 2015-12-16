#!/usr/bin/python
# -*- coding: UTF-8 -*-

import multiprocessing
import time
import sys
import os
import string
def scan(cmd):
  os.system(cmd)

pingcmd={}
outfile="result"
pingcmd[0]="./ping -s 172.10.10.1  -t 200 -c 1000 -n 3 -w -f %s" %(outfile)
pingcmd[1]="./ping -s 192.168.56.1  -t 200 -c 1000 -n 3 -w -f %s" %(outfile)
pid_num = len(pingcmd)
pool = multiprocessing.Pool(processes = pid_num)


os.system("echo `date` >"+outfile)
for i in range(0,pid_num):
  pool.apply_async(scan, (pingcmd[i], ))

pool.close()
pool.join()


