#!/usr/bin/python
# -*- coding: UTF-8 -*-

#import sys
#cmdline = sys.argv
#print cmdline
#del cmdline[0]
#print cmdline
#import os
#runline=""
#for argv in cmdline:
#    runline = runline + argv + " "
#print runline
#runline = runline + " > tdata.txt "
#os.system(runline)

#1cpu0=()
#1def cut_sys(filename):
#1    file = open(filename)
#1    lines = file.readlines()
#1    file.close()
#1    del lines[0]
#1    del lines[0]
#1    length=len(lines)
#1    del lines[length-1]
#1    del lines[0]
#1    cpu={}
#1    i=0
#1    for line in lines:
#1        linetmp = line.split()
#1        cpu[i] = linetmp[4]
#1        i=i+1
#1    return cpu
#1
#1cpu0=cut_sys("cpu0")
#1cpu1=cut_sys("cpu1")
#1
#1cpu0t=0
#1cpu1t=0
#1fileout=open("cpusys.csv","w")
#1
#1fileout.write("cpu0,cpu1\r\n")
#1length=len(cpu1)
#1
#1for i in range(0,length): 
#1    str="%s,%s\r\n" %(cpu0[i],cpu1[i])
#1    fileout.write(str)
#1    cpu0t=cpu0t+float(cpu0[i])
#1    cpu1t=cpu1t+float(cpu1[i])
#1fileout.close()
#1
#1print "cpu0t:%f,cpu1t:%f,cpu0t/cpu1t:%f" %(cpu0t/length,cpu1t/length,cpu0t/cpu1t) 
#1

import multiprocessing
import time
import sys
import os
import string
def scan(scan_ip,id):
  #str = "%.3d" %(id)
  #print str
  sys.stdout.write(scan_ip + "\r")
  #sys.stdout.write("#" + "->" + "\b\b")
  sys.stdout.flush()
  os.system("./ping3 "+scan_ip+">>" + seg_ip)

cmdline = sys.argv
seg_ip = cmdline[1]
pid_num = string.atoi(cmdline[2])
os.system("echo >"+seg_ip)
pool = multiprocessing.Pool(processes = pid_num)


for i in range(0,253):
  scan_ip = "%s.%.3d" %(seg_ip,i)
  pool.apply_async(scan, (scan_ip,i, ))

pool.close()
pool.join()


