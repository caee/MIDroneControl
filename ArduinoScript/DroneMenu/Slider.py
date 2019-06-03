import pygame
import sys

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 50, 50)
YELLOW = (255, 255, 0)
GREEN = (0, 255, 50)
BLUE = (50, 50, 255)
GREY = (200, 200, 200)
ORANGE = (200, 100, 50)
CYAN = (0, 255, 255)
MAGENTA = (255, 0, 255)
TRANS = (1, 1, 1)


class Slider():
    def __init__(self, screen, val, maxi, mini, posx, posy):
        self.screen = screen
        self.font = pygame.font.SysFont("monospace", 20)
        self.val = val  # start value
        self.maxi = maxi  # maximum at slider position right
        self.mini = mini  # minimum at slider position left
        self.xpos = posx  # x-location on screen
        self.ypos = posy
        self.surf = pygame.surface.Surface((50, 200))
        self.hit = False  # the hit attribute indicates slider movement due to mouse interaction

        #self.txt_surf = self.font.render(name, 1, BLACK)
        #self.txt_rect = self.txt_surf.get_rect(center=(5, 180))

        # Static graphics - slider background #
        self.surf.fill((100, 100, 100))
        pygame.draw.rect(self.surf, GREY, [0, 0, 50, 200], 3)
        pygame.draw.rect(self.surf, ORANGE, [10, 10, 30, 180], 0)

        #self.surf.blit(self.txt_surf, self.txt_rect)  # this surface never changes

        # dynamic graphics - button surface #
        self.button_surf = pygame.surface.Surface((100, 20))
        self.button_surf.fill(TRANS)
        self.button_surf.set_colorkey(TRANS)
        #pygame.draw.circle(self.button_surf, BLACK, (10, 10), 6, 0)
        #pygame.draw.circle(self.button_surf, ORANGE, (10, 10), 4, 0)
        pygame.draw.rect(self.button_surf, BLACK, [0, 0, 100, 20], 0)
        pygame.draw.rect(self.button_surf, GREEN, [2, 2, 96, 16], 0)

    def draw(self):
        """ Combination of static and dynamic graphics in a copy of
    the basic slide surface
    """
        # static
        surf = self.surf.copy()

        # dynamic
        pos = (22, 10+int((self.val-self.mini)/(self.maxi-self.mini)*180))
        self.button_rect = self.button_surf.get_rect(center=pos)
        surf.blit(self.button_surf, self.button_rect)
        self.button_rect.move_ip(self.xpos, self.ypos)  # move of button box to correct screen position

        # screen
        self.screen.blit(surf, (self.xpos, self.ypos))

    def move(self):
        """
    The dynamic part; reacts to movement of the slider button.
    """
        self.val = (pygame.mouse.get_pos()[1] - self.ypos - 10) / 180 * (self.maxi - self.mini) + self.mini
        if self.val < self.mini:
            self.val = self.mini
        if self.val > self.maxi:
            self.val = self.maxi