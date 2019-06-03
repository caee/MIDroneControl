## @package FlightView
#  Takes care of the flight control view
#
#  This module automatically sets up the flight view with the right buttons, graphics, images, etc.


import pygame
from MenuItem import MenuItem
from Slider import Slider
import math
import time
from Colors import *

class FlightView():
    """Class to to generate the flight control view

    How to use:
     1) Create a new object of this class and put the screen as parameter
     2) Now execute the function called run, which will run the mainloop until the application is closed

    """

    ## The constructor.
    #  @param Screen The window the flight view should be showed in.
    #  @param Connection The connection to the Arduino so that the view can communicate directly with it.
    def __init__(self, Screen, Connection):
        self.Screen = Screen
        self.Connection = Connection

        self.Clock = pygame.time.Clock()
        self.MouseIsVisible = True
        self.CurrentItem = None
        self.bMainLoop = True
        self.BackgroundColor = BLACK

    ## Sets if the mouse should be visible or not(used in the main loop).
    def SetMouseVisibility(self):
        if self.MouseIsVisible:
            pygame.mouse.set_visible(True)
        else:
            pygame.mouse.set_visible(False)

    ## Determine which button the mouse is hovering over(used in the main loop).
    def SetMouseSelection(self, item, mposx, mposy):
        if item.IsMouseSelection(mposx, mposy):
            item.SetFontColor(RED)
            item.set_italic(True)
        else:
            item.SetFontColor(WHITE)
            item.set_italic(False)

    ## Rotates an image around the center of it, instead of the corner as it will by default
    #  @param Image The image object to be rotated
    #  @param Rect The rectangle the image is currently child element of
    #  @param Angle The angle in degrees the image should be rotated
    def RotateAroundCenter(self, Image, Rect, Angle):
        RotatedImage = pygame.transform.rotozoom(Image, Angle, 1)
        RotatedRect = RotatedImage.get_rect(center=Rect.center)
        return RotatedImage, RotatedRect

    def IncThrottle(self):
        self.Throttle = self.Throttle + 1 if self.Throttle < 100 else 100

    ## Quick way to decrease the throttle by 1, but stops at 0
    def DecThrottle(self):
        self.Throttle = self.Throttle - 1 if self.Throttle > 0 else 0

    ## Quick way to increase the elevator by 1, but stops at 100
    def IncElevator(self):
        self.Elevator = self.Elevator + 1 if self.Elevator < 100 else 100

    ## Quick way to decrease the elevator by 1, but stops at 0
    def DecElevator(self):
        self.Elevator = self.Elevator -1 if self.Elevator > 0 else 0

    ## Quick way to increase the rudder by 1, but stops at 100
    def IncRudder(self):
        self.Rudder = self.Rudder + 1 if self.Rudder < 100 else 100

    ## Quick way to decrease the rudder by 1, but stops at 0
    def DecRudder(self):
        self.Rudder = self.Rudder - 1 if self.Rudder > 0 else 0

    ## Quick way to increase the aileron by 1, but stops at 100
    def incAileron(self):
        self.Aileron = self.Aileron + 1 if self.Aileron < 100 else 100

    ## Quick way to decrease the aileron by 1, but stops at 0
    def DecAileron(self):
        self.Aileron = self.Aileron - 1 if self.Aileron > 0 else 0

    ## Contains the main loop and only stops when bMainLoop = False
    def Run(self):
        self.bMainLoop = True

        self.Items = []

        # Slider for throttle
        self.ThrottleSlider = Slider(self.Screen, 90, 180, 1, 730, 50)

        # Buttons for Elevator
        MI = MenuItem("-", None, 20, WHITE)
        MI.SetPosition(100, 268)
        MI.SetAction(self.DecElevator)
        self.Items.append(MI)
        MI = MenuItem("+", None, 20, WHITE)
        MI.SetPosition(145, 268)
        MI.SetAction(self.IncElevator)
        self.Items.append(MI)

        # Buttons for Rudder
        MI = MenuItem("-", None, 20, WHITE)
        MI.SetPosition(340, 268)
        MI.SetAction(self.DecRudder)
        self.Items.append(MI)
        MI = MenuItem("+", None, 20, WHITE)
        MI.SetPosition(385, 268)
        MI.SetAction(self.IncRudder)
        self.Items.append(MI)

        # Buttons for Aileron
        MI = MenuItem("-", None, 20, WHITE)
        MI.SetPosition(580, 268)
        MI.SetAction(self.DecAileron)
        self.Items.append(MI)
        MI = MenuItem("+", None, 20, WHITE)
        MI.SetPosition(625, 268)
        MI.SetAction(self.incAileron)
        self.Items.append(MI)

        # Slider for sensivity
        self.sensivitySlider = Slider(self.Screen, 165, 180, 1, 830, 50)

        # Font object used to convert strings to graphically insertable element in pygame
        Font = pygame.font.SysFont("monospace", 20)

        # We load the images in advance, to it does not reload them in the main loop each time
        self.ImageDroneTop = pygame.image.load('Images/droneTop.png')
        self.ImageDroneTopRect = self.ImageDroneTop.get_rect(center=(120, 150))
        self.DroneTopRotate = 0;

        self.ImageDroneSide = pygame.image.load('Images/droneSide.png')
        self.ImageDroneSideRect = self.ImageDroneSide.get_rect(center=(360, 150))

        self.ImageDroneFront = pygame.image.load('Images/droneFront.png')
        self.ImageDroneFrontRect = self.ImageDroneFront.get_rect(center=(600, 150))


        # The initial default values
        self.Throttle = 50  # Throttle
        self.Elevator = 50  # Elevator/Pitch
        self.Rudder = 50    # Rudder/yaw
        self.Aileron = 50   # Aileron/Roll
        self.Sensivity = 20  # How much will the controls change for each key event?
        sens = 20


        LastThrottleSent = 0
        LastElevatorSent = 0
        LastRudderSent = 0
        LastAileronSent = 0
        LastTimeSent = time.time()

        # Send output to Arduino in case of true. Used to test arduino reset mechanism.
        bSendOutput = True;

        while self.bMainLoop:

            # The temporary value if key is pressed, these values will be sent to the Arduino
            NewThrottle = self.Throttle
            NewElevator = self.Elevator
            NewRudder = self.Rudder
            NewAileron = self.Aileron

            # The actions to be done according to a key press
            Key = pygame.key.get_pressed()

            if Key[pygame.K_w]:
                NewElevator += sens
                # NewElevator += self.Sensivity
                print(NewElevator)
            if Key[pygame.K_s]:
                NewElevator -= sens
                print(NewElevator)
            if Key[pygame.K_d]:
                NewAileron += sens
            if Key[pygame.K_a]:
                NewAileron -= sens
            if Key[pygame.K_RIGHT]:
                NewRudder += sens
            if Key[pygame.K_LEFT]:
                NewRudder -= sens
            if Key[pygame.K_o]:
                bSendOutput = not(bSendOutput)
                

            # Draw the background
            self.Screen.fill(self.BackgroundColor)

            # Graphics for throttle
            self.ThrottleSlider.draw()

            if(self.ThrottleSlider.hit):
                self.ThrottleSlider.move()
                # Takes the slider value at converts it to an integer interval from 0 to 100
                self.Throttle = math.ceil(((180 - self.ThrottleSlider.val) / 180) * 100)
            else:
                self.Throttle = NewThrottle;
            self.Screen.blit(Font.render(str(self.Throttle), 1, WHITE), (740, 265))



            # Graphics for Rudder
            self.DroneTopRotate+= (NewRudder - 50) * -0.1
            newImg, newRect = self.RotateAroundCenter(self.ImageDroneTop, self.ImageDroneTopRect, self.DroneTopRotate)
            self.Screen.blit(newImg, newRect)
            self.Screen.blit(Font.render(str(NewRudder), 1, WHITE), (110, 265))

            # Graphics for elevator
            newImg, newRect = self.RotateAroundCenter(self.ImageDroneSide, self.ImageDroneSideRect, 50 - NewElevator)
            self.Screen.blit(newImg, newRect)
            self.Screen.blit(Font.render(str(NewElevator), 1, WHITE), (350, 265))

            # Graphics for aileron
            newImg, newRect = self.RotateAroundCenter(self.ImageDroneFront, self.ImageDroneFrontRect, 50 - NewAileron)
            self.Screen.blit(newImg, newRect)
            self.Screen.blit(Font.render(str(NewAileron), 1, WHITE), (590, 265))

            # Graphics for sensivity
            self.sensivitySlider.draw()
            if(self.sensivitySlider.hit):
                self.sensivitySlider.move()
            # Takes the slider value at converts it to an integer interval from 0 to 100
            self.Sensivity = math.ceil(((180-self.sensivitySlider.val)/180)*50)
            self.Screen.blit(Font.render(str(self.Sensivity), 1, WHITE), (840, 265))


            # Limit frame rate to 50 FPS
            self.Clock.tick(50)

            MousePos = pygame.mouse.get_pos()

            for Event in pygame.event.get():
                if Event.type == pygame.QUIT:
                    self.bMainLoop = False

                # See if someonething should happen if mouse is clicked
                if Event.type == pygame.MOUSEBUTTONDOWN:
                    pos = pygame.mouse.get_pos()
                    if self.ThrottleSlider.button_rect.collidepoint(pos):
                        self.ThrottleSlider.hit = True
                    if self.sensivitySlider.button_rect.collidepoint(pos):
                        self.sensivitySlider.hit = True
                    for Item in self.Items:
                        if Item.IsMouseSelection(MousePos[0], MousePos[1]):
                            Item.action()
                # Now make sure they reset.
                if Event.type == pygame.MOUSEBUTTONUP:
                    self.ThrottleSlider.hit = False
                    self.sensivitySlider.hit = False

            if pygame.mouse.get_rel() != (0, 0):
                self.MouseIsVisible = True
                self.CurrentItem = None

            self.SetMouseVisibility()

            for Item in self.Items:
                if self.MouseIsVisible:
                    self.SetMouseSelection(Item, MousePos[0], MousePos[1])
                self.Screen.blit(Item.Label, Item.Position)



            pygame.display.flip()

            #### Communication with drone ####

            # If one or more values are not the same as the values sent to Arduino earlier,
            # then we should send it
            # Everything is send, if it has been more than 2 seconds since last time something was sent
            # to prevent that the Arduino do not reset
            ArduinoInput = ""
            if(NewThrottle != LastThrottleSent or time.time()-LastTimeSent > 4):
                ArduinoInput+="T"+self.NumberToString(NewThrottle, 3)
            if(NewElevator != LastElevatorSent or time.time()-LastTimeSent > 4):
                ArduinoInput+="E"+self.NumberToString(NewElevator, 3)
            if(NewRudder != LastRudderSent or time.time()-LastTimeSent > 4):
                ArduinoInput+="R"+self.NumberToString(NewRudder, 3)
            if (NewAileron != LastAileronSent or time.time()-LastTimeSent > 4):
                ArduinoInput+="A"+self.NumberToString(NewAileron, 3)

            # Macros, known to the arduino so far.
            bIsOutputMacro = False
            if(self.Throttle == NewThrottle and self.Elevator == NewElevator and self.Rudder == NewRudder and self.Aileron == NewAileron):
                if Key[pygame.K_0]:
                    ArduinoInput = "M000"
                    bIsOutputMacro = True
                elif Key[pygame.K_1]:
                    # ArduinoInput = "M001" # Kind of dangerous to do, since this is supposed to start the drone, outcommented so far
                    ArduinoInput = "M000"
                    bIsOutputMacro = True

            # If there is something to send or it has been more than 2 seconds since something was send
            # then do the following
            # Toggle output
            if ( ( (len(ArduinoInput) > 0) or time.time()-LastTimeSent > 4 ) and (bSendOutput) ):

                # Sends out the changes to the Arduino
                try:
                    ArduinoInput = '<' + ArduinoInput + '>';
                    print("Input:")
                    print(ArduinoInput);
                    print("")
                    print(ArduinoInput.encode())
                    self.Connection.write(ArduinoInput.encode());
                    LastTimeSent = time.time()
                except Exception as e:
                    print("Output error: ", e)

                # Receives a response from the Arduino
                try:
                    bForceWait = True       # If given a hold command this will be essential
                    BufferCount = self.Connection.in_waiting
                    while (bForceWait or BufferCount > 0):
                        # While nothing is received from the Arduino, we should wait for the Arduino to answer
                        # to prevent sending new things before the Arduino is ready
                        while (BufferCount == 0):
                            BufferCount = self.Connection.in_waiting
                            time.sleep(0.001)

                        # Now that we got something from the arduino again, reset controls
                        if ( bIsOutputMacro ):

                            bIsOutputMacro = False
                            NewThrottle = 50

                        SerialOut = self.Connection.read().decode()

                        while ('\n' not in SerialOut[-1]):
                            SerialOut += self.Connection.read().decode()  # The data comes as a byte value, interpret it.

                        print("Output: ")
                        print(SerialOut)
                        print("")
                        bForceWait = False
                        BufferCount = self.Connection.in_waiting

                    # Sets the last values send and processed by the Arduino
                    LastThrottleSent = NewThrottle
                    LastElevatorSent = NewElevator
                    LastRudderSent = NewRudder
                    LastAileronSent = NewAileron
                except Exception as e:
                    print("Input error: ", e)

    ## Stops the main loop
    def Stop(self):
        self.bMainLoop = False

    ## Gives a string from a number and adds zeros in front so that the string always has a certain length
    #  @param Number The number to convert to a string
    #  @param Length The fixed length that the string should have
    def NumberToString(self, Number, Length):
        numberStr = str(Number)
        while(len(numberStr) < Length):
            numberStr = "0"+numberStr
        return numberStr