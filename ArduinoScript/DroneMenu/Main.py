#!/usr/bin/python

import sys
import pygame
from MenuView import MenuView
from FlightView import FlightView

import ArduinoIO

pygame.init()


def findDevices(MV):
    print(ArduinoIO)
    funcs = {}      # Clean menu,
    ActivePorts = ArduinoIO.SerialPorts()
    print(ActivePorts)
    # Get list of active ports containing the property "Arduino".
    for i in range(len(ActivePorts)):
        if(ActivePorts[i]):
            # Print out serial ports as menu items
            funcs[ActivePorts[i]] = lambda: startFlight(MV,ActivePorts[i])
    funcs["Quit"] = sys.exit # exit for the port selection sub menu
    MV.Refresh(funcs.keys(), funcs)

def startFlight(MV, port):
    con = ArduinoIO.SerialConnection(port)
    MV.Stop()
    fv = FlightView(MV.Screen, con)
    fv.Run()


# Creating the screen
screen = pygame.display.set_mode((900, 400), 0, 32)


pygame.display.set_caption('Drone Control')
MV = MenuView(screen)
funcs = {
         "Connect to Arduino": lambda: findDevices(MV),
         "Quit": sys.exit
         }
try:                                    
    MV.Refresh(funcs.keys(), funcs)
    MV.Run()
except SystemExit:                  #Exception 
        pygame.quit()
#fv = FlightView(screen, None)
#fv.run()