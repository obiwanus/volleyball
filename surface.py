# coding: utf-8
from __future__ import unicode_literals, division, absolute_import, print_function
from collections import namedtuple
import sys
import pygame
import random
from pygame.locals import *


# Initialization
pygame.init()

BG_COLOR = pygame.Color(0, 0, 0)
WINDOW_SIZE = WIDTH, HEIGHT = 650, 500
FPS = 10

screen = pygame.display.set_mode(WINDOW_SIZE, DOUBLEBUF)
screen.fill(BG_COLOR)
pygame.display.set_caption("Surface")
clock = pygame.time.Clock()


# Generate dots
DOTS_NUM = 1000
DOT_COLOR = GREEN = pygame.Color(50, 200, 50)
Dot = namedtuple('Dot', 'x y')
dots = set()
for i in xrange(DOTS_NUM):
    dots.add(Dot(random.randrange(WIDTH), random.randrange(HEIGHT)))
dots = list(dots)
dots.sort(key=lambda p: -p.y)  # sort by Y (descending)

# Print the dots
pixels = pygame.PixelArray(screen)
for dot in dots:
    pixels[dot.x][dot.y] = DOT_COLOR
del pixels
pygame.display.update()


pixels = pygame.PixelArray(screen)

while True:
    clock.tick(FPS)
    for event in pygame.event.get():
        if event.type == QUIT:
            del pixels
            pygame.quit()
            sys.exit(0)
        if not hasattr(event, 'key'):
            continue
        if event.key == K_ESCAPE:
            del pixels
            pygame.quit()
            sys.exit(0)

    # Drop the dots
    for i, dot in enumerate(dots):
        if dot.y < HEIGHT - 1 and pixels[dot.x][dot.y + 1] == screen.map_rgb(BG_COLOR):
            # Move the dot down
            pixels[dot.x][dot.y + 1] = pixels[dot.x][dot.y]
            pixels[dot.x][dot.y] = BG_COLOR
            dots[i] = Dot(dot.x, dot.y + 1)
    pygame.display.update()
