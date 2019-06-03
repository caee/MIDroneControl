# -*- coding: utf-8 -*-
"""
Created on Wed Mar  6 11:43:59 2019

@author: carle
"""
import serial;
import serial.tools.list_ports;

ActivePorts = [d.device for d in serial.tools.list_ports.comports()]
PortProduct = [d.product for d in serial.tools.list_ports.comports()]
print(ActivePorts)
print(PortProduct)
test = serial.tools.list_ports.comports()
print(test.product)