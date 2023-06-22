#!/usr/bin/env python
# -*- coding:UTF-8 -*-
from __future__ import print_function
import  serial

if __name__ == '__main__':
        listdata = []
        lastangle = 0
        # LD19波特率：230400     LD14波特率：115200
        ser = serial.Serial('/dev/wheeltec_lidar', 115200)  # ubuntu，如果未修改串口别名，可通过 ll /dev 查看雷达具体端口再进行更改
        # ser = serial.Serial("COM5", 115200, timeout=5)    # window系统，需要先通过设备管理器确认COM口
        while True:
                 data = ser.read(2)                                 # 读取2个字节数据
                 if ord(data[0]) == 0x54 and ord(data[1]) == 0x2C:  # 判断是否为数据帧头
                    data = ser.read(45)                             # 是数据帧头就读取整一帧，去掉帧头之后为45个字节

                    # for x in range(44):                           # 打印原始数据，默认不打印
                    #  print('%#.2x '%ord(data[x]),end="\t")
                    # print("\n")

                    listdata.insert(0, "转速（度/秒）:")
                    listdata.insert(1, ord(data[1])*256+ord(data[0]))    # 转速：高字节在后，低字节在前，复合后再转换成十进制

                    listdata.insert(2, "起始角度（度）:")
                    listdata.insert(3, (ord(data[3])*256+ord(data[2]))/100.0)  # 原始角度为方便传输放大了100倍，这里要除回去
                    listdata.insert(4, "距离（mm）|光强 *12个点 :")
                    j = 5
                    for x in range(4, 40, 3):                                 # 2个字节的距离数据，1个信号强度数据，步长为3
                        listdata.insert(j, ord(data[x+1])*256+ord(data[x]))  # 距离
                        j += 1
                        listdata.insert(j, ord(data[x+2]))                   # 信号强度
                        j += 1

                    listdata.insert(29, "结束角度（度）:")
                    listdata.insert(30, (ord(data[41])*256+ord(data[40]))/100.0)
                    listdata.insert(31, "时间戳（ms）:")
                    listdata.insert(32, ord(data[43])*256+ord(data[42]))
                    j = 33

                    if lastangle - listdata[3] > 100:         # 判断上一帧的起始角度与这一帧的角度差是否大于一定角度即为新的一圈
                      print("*******************************")
                    lastangle = listdata[3]                   # 将此帧角度赋值为上一帧，为下一次的判断做准备
                    for k in range(j):
                        print(listdata[k], end="\t")          # 打印解析之后的数据
                    print("\n")
