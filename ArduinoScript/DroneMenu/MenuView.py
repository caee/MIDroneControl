## @package MenuView
#  Takes care of the first menu
#
#  This module contains the class MenuView, which makes it possible easily to add new buttons in
#  the main menu

import pygame
from MenuItem import MenuItem
from Colors import *


class MenuView():
    """Class to generate a vertical menu.

    How to use:
     1) Create a new object of this class and put the screen as parameter
     2) Call the class function refresh to add elements in the menu.
     3) Now execute the function called run, which will block until a menu button has been clicked

    """

    ## The constructor.
    #  @param Screen The window the menu should be showed in.
    def __init__(self, Screen):
        self.Screen = Screen
        self.ScreenWidth = self.Screen.get_rect().width
        self.ScreenHeight = self.Screen.get_rect().height
        self.Clock = pygame.time.Clock()
        self.bMouseIsVisible = True
        self.CurrentItem = None
        self.bMainLoop = True
        self.Items = []

    ## To set the mouse visibility in the window according to the global variable bMouseIsVisible.
    def SetMouseVisibility(self):
        if self.bMouseIsVisible:
            pygame.mouse.set_visible(True)
        else:
            pygame.mouse.set_visible(False)

    ## Changes the select menu item if arrow key up or down has been pressed.
    #  @param Key The key which was pressed.
    def SetKeyboardSelection(self, Key):
        for Item in self.Items:
            # Deselect all items
            Item.set_italic(False)
            Item.SetFontColor(WHITE)

        if self.CurrentItem is None:
            self.CurrentItem = 0
        else:
            # Gets the key pressed and choose the current selection accordingly
            if Key == pygame.K_UP and self.CurrentItem > 0:
                self.CurrentItem -= 1
            elif Key == pygame.K_UP and self.CurrentItem == 0:
                self.CurrentItem = len(self.Items) - 1
            elif Key == pygame.K_DOWN and self.CurrentItem < len(self.Items) - 1:
                self.CurrentItem += 1
            elif Key == pygame.K_DOWN and self.CurrentItem == len(self.Items) - 1:
                self.CurrentItem = 0

        self.Items[self.CurrentItem].set_italic(True)
        self.Items[self.CurrentItem].SetFontColor(RED)

        # If enter or space is pressed, run the the function linked to the item
        if Key == pygame.K_SPACE or Key == pygame.K_RETURN:
            Text = self.Items[self.CurrentItem].Text
            self.Funcs[Text]()

    ## Selects the item currently selected.
    #  @param Item The item which should be selected
    #  @param MousePosX The current x position of the mouse
    #  @param MousePosX The current y position of the mouse
    def SetMouseSelection(self, Item, MousePosX, MousePosY):
        if Item.IsMouseSelection(MousePosX, MousePosY):
            Item.SetFontColor(RED)
            Item.set_italic(True)
        else:
            Item.SetFontColor(WHITE)
            Item.set_italic(False)

    ## Refreshes/Sets the items the menu should have.
    #  @param Items A MenuItem array with all the items which should be in the menu
    #  @param Funcs The function pointer array of every MenuItem in the same order as Items
    #  @param BackgroundColor(optional) The background color of the entire window - default=BLACK
    #  @param Font(optional) The font used by the MenuItem - default=None
    #  @param FontSize(optional) The font size used by MenuItem - default=30
    #  @param FontColor(optional) The font color used by MenuItem - default=WHITE
    def Refresh(self, Items, Funcs, BackgroundColor=BLACK, Font=None, FontSize=30, FontColor=WHITE):
        self.BackgroundColor = BackgroundColor
        self.Funcs = Funcs
        self.Items = []
        self.CurrentItem = None
        for Index, Item in enumerate(Items):
            MI = MenuItem(Item, Font, FontSize, FontColor)
            TotalHeight = len(Items) * MI.Height
            PosX = (self.ScreenWidth / 2) - (MI.Width / 2)

            #Formular to place all items in the center of the screen,
            # but according to the item order in the MenuItem array given
            PosY = (self.ScreenHeight / 2) - (TotalHeight / 2) + ((Index * 2) + Index * MI.Height)

            MI.SetPosition(PosX, PosY)
            self.Items.append(MI)

    ## The function containing the mainloop which is listening to keyboard inputs
    # Blocks until an item is chosed(Enter or Space is pressed)
    def Run(self):
        self.bMainLoop = True
        while self.bMainLoop:
            # Limit frame speed to 50 FPS
            self.Clock.tick(50)

            MousePos = pygame.mouse.get_pos()

            for Event in pygame.event.get():
                if Event.type == pygame.QUIT:
                    self.bMainLoop = False
                if Event.type == pygame.KEYDOWN:
                    self.bMouseIsVisible = False
                    self.SetKeyboardSelection(Event.key)
                if Event.type == pygame.MOUSEBUTTONDOWN:
                    for Item in self.Items:
                        if Item.IsMouseSelection(MousePos[0], MousePos[1]):
                            self.Funcs[Item.Text]()

            if pygame.mouse.get_rel() != (0, 0):
                self.bMouseIsVisible = True
                self.CurrentItem = None

            self.SetMouseVisibility()

            # Redraw the background
            self.Screen.fill(self.BackgroundColor)

            for Item in self.Items:
                if self.bMouseIsVisible:
                    self.SetMouseSelection(Item, MousePos[0], MousePos[1])
                self.Screen.blit(Item.Label, Item.Position)

            pygame.display.flip()

    ## Goes out of the main loop by setting the variable bMainLoop to false
    def Stop(self):
        self.bMainLoop = False;