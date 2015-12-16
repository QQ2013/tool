#!/usr/bin/python
# -*- coding: UTF-8 -*-

import multiprocessing
import time
import sys
import os
import string


cmdline = sys.argv
start_i = string.atoi(cmdline[1])
end_i   = string.atoi(cmdline[2])
pid_num = string.atoi(cmdline[3])


for k in range(start_i,end_i+1):
  cmdline="./ping1.py %d %d" %(k,pid_num)
  os.system(cmdline)
