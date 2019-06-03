## @package ArduinoIO
#  A module containing the functions to find and connect to an Arduino

import serial;
import serial.tools.list_ports;
import time


## Gets the serial ports connected to the computer
#  Originally written by Thomas on Stackoverflow, modified as needed
# http://stackoverflow.com/questions/12090503/listing-available-com-ports-with-python
def SerialPorts():
    ActivePorts = [d.device for d in serial.tools.list_ports.comports()]
    PortDescription = [d.description for d in serial.tools.list_ports.comports()] #Changed to description instead of product. This is due to Win10 not recognizing product

    for i in range(len(ActivePorts)):
        if (PortDescription[i]):  # Necessary because of NoneType
            if ("Arduino" not in PortDescription[i]): 
                del ActivePorts[i]
        else:
            del ActivePorts[i]
    return ActivePorts

## Makes a connection to the Arduino
#  @param SerialPort The serial port the Arduino is connected to.
def SerialConnection(SerialPort):
    try:
        print("Connecting to Arduino.")
        # TODO?: Set dsrdtr to False when debug is finished? Perhaps not
        Ser = serial.Serial(SerialPort, 9600, dsrdtr=True, timeout = 0.5)
        time.sleep(1);

    except Exception as e:
        print("Couldn't connect to port. Please make sure correct port has been selected. Returning to main menu.");
        print("Error: ", e)
        return
    return Ser
