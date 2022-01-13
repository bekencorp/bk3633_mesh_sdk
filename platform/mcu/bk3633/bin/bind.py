#!/usr/bin/env python
#-*- coding:UTF-8 -*-

out = open("bindAB.bin", "wb")
print "新建输出文件：", out.name

in1 = open("a.bin", "rb")
print "读取文件：", in1.name

in2 = open("b.bin", "rb")
print "读取文件：", in2.name

Bsec_addr = 0x40000
tap = 0xff

def main():
    len = 0
    print "写入文件：", in1.name
    while 1:
        data = in1.read(1)
        len += 1
        if not data:
            break
        out.write(data)
    in1.close()
    print "插入间隔：0x%x" %(tap)
    while len <= Bsec_addr:
        out.write(chr(tap))
        len += 1
    print "写入文件：", in2.name
    while 1:
        data = in2.read(1)
        len += 1
        if not data:
            break
        out.write(data)
    in2.close()
    print "完成"
    out.close()

if __name__ == "__main__":
    main()
