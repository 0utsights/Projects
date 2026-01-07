import pygame, math

pygame.init()
screen = pygame.display.set_mode((800, 600))
clock = pygame.time.Clock()

size = 40
horiz = math.sqrt(3) * size      # center-to-center distance (east/west)
vert  = 1.5 * size               # center-to-center distance (down/up)

def hex_points(cx, cy, size):
    pts = []
    for i in range(6):
        a = math.radians(60*i - 30)  # pointy-top rotation
        pts.append((cx + size*math.cos(a), cy + size*math.sin(a)))
    return pts

# one hex center + one neighbor center using the spacing formulas
cx, cy = 250, 250
cx2, cy2 = cx + horiz, cy + vert

running = True
while running:
    clock.tick(60)
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            running = False

    screen.fill((20, 20, 20))
    pygame.draw.polygon(screen, (120, 200, 255), hex_points(cx,  cy,  size), 2)
    pygame.draw.polygon(screen, (255, 180, 120), hex_points(cx2, cy2, size), 2)
    pygame.display.flip()

pygame.quit()
