## @package MenuItem
#  Contains the function for every item/button in the MenuView and FlightView

import pygame
from Colors import *

class MenuItem(pygame.font.Font):
    """Class which facilitate the construction of a button or menu
    """

    ## The constructor.
    #  @param Text The name of the button.
    #  @param Font(optional) The font which should be used for the button - default None
    #  @param FontSize(optional) The font size - default 30
    #  @param PosX(optional) The horizontal position of the item in pixels - default 0
    #  @param PosY(optional) The vertical position of the item in pixels - default 0
    def __init__(self, Text, Font=None, FontSize=30, FontColor=WHITE, PosX=0, PosY=0):
        pygame.font.Font.__init__(self, Font, FontSize)
        self.Text = Text
        self.FontSize = FontSize
        self.FontColor = FontColor
        self.Label = self.render(self.Text, 1, self.FontColor) # The font object
        self.Width = self.Label.get_rect().width
        self.Height = self.Label.get_rect().height
        self.Dimensions = (self.Width, self.Height)
        self.PosX = PosX
        self.PosY = PosY
        self.Position = PosX, PosY

    ## Verifies if the mouse is over the item.
    #  @param PosX The x position of the mouse pointer
    #  @param PosY The y position of the mouse pointer
    def IsMouseSelection(self, PosX, PosY):
        if (PosX >= self.PosX and PosX <= self.PosX + self.Width) and \
                (PosY >= self.PosY and PosY <= self.PosY + self.Height):
            return True
        return False

    ## Sets the position of the item
    #  @param X The x position
    #  @param Y The y position
    def SetPosition(self, X, Y):
        self.Position = (X, Y)
        self.PosX = X
        self.PosY = Y

    ## Sets the font color of the item
    #  @param RGB The color in RGB format (r,g,b)
    def SetFontColor(self, RGB):
        self.FontColor = RGB
        self.Label = self.render(self.Text, 1, self.FontColor)

    ## Sets the function pointer which should fire when
    #  @param RGB The color in RGB format (r,g,b)
    def SetAction(self, Func):
        self.action = Func