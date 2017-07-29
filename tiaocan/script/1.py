#!/usr/bin/env python

#  **********************************************************************
#  the loop num is :   loop times
#  **********************************************************************
#  time:  running time
#  result for case-xyz:    it1  it2  it3  it4
#  item : 
    #  xyz:     x->c1      y->c2       z->l
    #  it1:    elite
    #  it2:    mul
    #  cost:   cost
#  **********************************************************************

import os
import commands

C1 = ['2']  # the size of case
C2 = ['1'] # the num of the case
C3 = ['5'] # elite
C4 = ['60','65','70','75','80','85','90','95'] # mul
C5 = ['2'] # types of item
loop = ['0','1','2'] # the times for the running

for c1 in C1:
    for c2 in C2:
        for c3 in C3:
            for c4 in C4:
                for c5 in C5:
                    for l in loop:
                        c6 = c1+c2+l
                        cmd = './cdn ./%s/case%s.txt ./result/%s/result%s.txt %s %s %s %s >> ./log1.txt' % (c1,c2,c1,c2,c3,c4,c5,c6)
                        print cmd
                        result = commands.getoutput(cmd)
#  print cmd

#  result = commands.getoutput(cmd)

