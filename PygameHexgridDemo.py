import pygame, math

pygame.init()
screen = pygame.display.set_mode((800, 600))
clock = pygame.time.Clock()

size = 40  # outer radius ("size" on Red Blob)

def axial_to_pixel(q, r, s):
    # pointy-top axial -> pixel (Red Blob)
    x = math.sqrt(3) * s * (q + r/2)
    y = (3/2) * s * r
    return x, y

def hex_points(cx, cy, s):
    pts = []
    for i in range(6):
        a = math.radians(60*i - 30)  # pointy-top
        pts.append((cx + s*math.cos(a), cy + s*math.sin(a)))
    return pts

origin = (0, 0)
east   = (1, 0)  # adjacent neighbor
se     = (0, 1)  # adjacent neighbor (southeast in axial)

running = True
while running:
    clock.tick(60)
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            running = False

    screen.fill((20, 20, 20))

    for (q, r), color in [(origin, (120,200,255)), (east, (255,180,120)), (se, (180,255,140))]:
        x, y = axial_to_pixel(q, r, size)
        x += 400; y += 300  # center on screen
        pygame.draw.polygon(screen, color, hex_points(x, y, size), 2)

    pygame.display.flip()

pygame.quit()
