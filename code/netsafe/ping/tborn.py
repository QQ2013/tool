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

cpu0=()
def cut_sys(filename):
    file = open(filename)
    lines = file.readlines()
    file.close()
    del lines[0]
    del lines[0]
    length=len(lines)
    del lines[length-1]
    del lines[0]
    cpu={}
    i=0
    for line in lines:
        linetmp = line.split()
        cpu[i] = linetmp[4]
        i=i+1
    return cpu

cpu0=cut_sys("cpu0")
cpu1=cut_sys("cpu1")

cpu0t=0
cpu1t=0
fileout=open("cpusys.csv","w")

fileout.write("cpu0,cpu1\r\n")
length=len(cpu1)

for i in range(0,length): 
    str="%s,%s\r\n" %(cpu0[i],cpu1[i])
    fileout.write(str)
    cpu0t=cpu0t+float(cpu0[i])
    cpu1t=cpu1t+float(cpu1[i])
fileout.close()

print "cpu0t:%f,cpu1t:%f,cpu0t/cpu1t:%f" %(cpu0t/length,cpu1t/length,cpu0t/cpu1t) 

