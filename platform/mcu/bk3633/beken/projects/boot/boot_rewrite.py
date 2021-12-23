#!/usr/bin/env python
#-*- coding:UTF-8 -*-
import socket  
from select import select  
import time
import os
import sys
import threading
import json
import struct
import binascii
import time
import re
import fileinput
import io
import logging

replace = {
"boot_func" :"0x0",
}

def traverse(f):
    fs = os.listdir(f)
    for f1 in fs:
        tmp_path = os.path.join(f, f1)
        if not os.path.isdir(tmp_path):
            if os.path.isfile(tmp_path):
                if isinstance(tmp_path, str):
                    if tmp_path.endswith('bk3633_boot.map'): 
                        logger.info(tmp_path)
                        find_str(tmp_path)
        else:
            traverse(tmp_path)
            
def traverse_for_replace(f):
    fs = os.listdir(f)
    for f1 in fs:
        tmp_path = os.path.join(f, f1)
        if not os.path.isdir(tmp_path):
            if os.path.isfile(tmp_path):
                if isinstance(tmp_path, str):
                    if tmp_path.endswith('stack_rom_symbol.txt'): 
                        logger.info(tmp_path)
                        replace_str(tmp_path)
        else:
            traverse(tmp_path)

def find_str(file):
    with io.open(file, 'r', encoding='utf-8') as fd:
        for line in fd:
            for key, value in replace.items():
                if re.search(key, line.strip()) != None:
                    print(line)
                    logger.info(line)
                    replace[key] = "0x"+line.split('0x')[1].split(' ')[0]
    fd.close()
    logger.info(str(replace))

def replace_str(file): 
    with io.open(r'tmp.c', 'w', encoding='utf-8') as fw:
        with io.open(file, 'r', encoding='utf-8') as fd:
            for line in fd:
                replace_flag = True
                for key, value in replace.items():
                    if re.search(key, line.strip()) != None:
                        replace_flag = False
                        fw.write(key + ' = '+ value + ';')
                        fw.write(u'\n')
                        logger.info(key + ' = '+ value + ';')
                if replace_flag :
                    fw.write(line)
                
        fd.close()
    fw.close()
        
    with io.open(file, 'w', encoding='utf-8') as fw:
        with io.open(r'tmp.c', 'r', encoding='utf-8') as file:
            for line in file:
                fw.write(line)

    
if __name__ == "__main__":
    if(os.path.exists("log.txt")):
        os.remove("log.txt")
    logger = logging.getLogger(__name__)
    logger.setLevel(level = logging.INFO)
    handler = logging.FileHandler("log.txt")
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    
    traverse("./")
    traverse_for_replace("../../../bin/")










