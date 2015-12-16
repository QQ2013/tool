#!/usr/bin/python
# -*- coding: UTF-8 -*-

import multiprocessing
import time
import sys
import os
import string

def scan(scan_ip,seg_ip):
  sys.stdout.write(scan_ip + "\r")
  sys.stdout.flush()
  os.system("./ping3 "+scan_ip+">>" + seg_ip)

cmdline = sys.argv
start_i = string.atoi(cmdline[1])
end_i   = string.atoi(cmdline[2])
pid_num = string.atoi(cmdline[3])
pool = multiprocessing.Pool(processes = pid_num)

#exit()

for k in range(start_i,end_i):
  seg_ip = "%d" %(k)
  os.system("echo >"+seg_ip)
  for i in range(0,253):
    for j in range(0,253):
      scan_ip = "%.3d.%.3d.%.3d" %(k,i,j)
      pool.apply_async(scan, (scan_ip,seg_ip, ))

pool.close()
pool.join()
