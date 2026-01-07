import pygame, math
pygame.init()
screen = pygame.display.set_mode((800, 600))

size = 40 # outer radius of hexagon from pointy angles

# HEXAGON FORMULA

def axial_to_pixel(q, r, size):
    x = math.sqrt(3) * size * (q+r/2)
    y = 3/2 * size * r
    return x,y

def hex_points(cx, cy, size):
    points = []
    for i in range(6): # 6 corners
        angle = math.radians(60 * i - 30) # pointy top
        x = cx + size * math.cos(angle)
        y = cy + size * math.sin(angle)
        points.append((x, y))
    return points

hexes = [
    (0,0),
    (1,0),
    (0,1),
    (1,1),
    (-1,0),
    (0,-1),
    (-1,-1),
    (1,-1),
    (-1,1)
]

# GAME LOOP

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    screen.fill((20,20,20))

    for q, r in hexes:
        x, y = axial_to_pixel(q,r,size)

        #center the hexgrid in relation to screen size
        x += 400
        y += 300

        pygame.draw.polygon(screen, (120, 200, 255), hex_points(x,y,size), 2)

    pygame.display.flip()

pygame.quit()
