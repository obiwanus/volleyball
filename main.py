import pygame
import sys
from pygame.locals import *


# Init
pygame.init()
WIDTH = 500
HEIGHT = 400
screen = pygame.display.set_mode((WIDTH, HEIGHT), DOUBLEBUF)
pygame.display.set_caption('Tank wars')
clock = pygame.time.Clock()
FPS = 30
ball = pygame.image.load('ball.png')
BALL_SIZE = 15
BLACK = (0, 0, 0)
k_left = k_right = k_up = k_down = 0
speed_x = speed_y = 3
ACC = 2
MAX_SPEED = 30
x, y = 100, 100
MAX_X, MAX_Y = WIDTH - BALL_SIZE, HEIGHT - BALL_SIZE


def sign(x):
    return 1 if x >= 0 else -1


# Main game loop
while True:
    deltat = clock.tick(FPS)
    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit(0)
        if not hasattr(event, 'key'):
            continue
        down = event.type == KEYDOWN
        if event.key == K_LEFT:
            k_left = down * ACC
        elif event.key == K_RIGHT:
            k_right = down * ACC
        elif event.key == K_UP:
            k_up = down * ACC
        elif event.key == K_DOWN:
            k_down = down * ACC
        elif event.key == K_ESCAPE:
            pygame.quit()
            sys.exit(0)
    screen.fill(BLACK)

    # Simulation
    speed_x += (k_right - k_left)
    speed_y += (k_down - k_up)
    if abs(speed_x) > MAX_SPEED:
        speed_x = sign(speed_x) * MAX_SPEED
    if abs(speed_y) > MAX_SPEED:
        speed_y = sign(speed_y) * MAX_SPEED
    if x > MAX_X:
        speed_x = -speed_x
        x = MAX_X
    elif x < 0:
        speed_x = -speed_x
        x = 0
    if y > MAX_Y:
        speed_y = -speed_y
        y = MAX_Y
    elif y < 0:
        speed_y = -speed_y
        y = 0

    position = x, y = x + speed_x, y + speed_y
    screen.blit(ball, position)
    pygame.display.flip()

