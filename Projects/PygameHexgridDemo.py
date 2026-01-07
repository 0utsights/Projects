import pygame
import math

# ---------------- CONFIG ----------------
WIDTH, HEIGHT = 800, 600
HEX_SIZE = 30
RADIUS = 3

BG_COLOR = (20, 20, 20)
HEX_COLOR = (120, 200, 255)
OUTLINE_COLOR = (30, 30, 30)

# ---------------- HEX MATH ----------------
# POINTY-TOP axial -> pixel
def axial_to_pixel(q, r, size):
    x = size * math.sqrt(3) * (q + r / 2)
    y = size * 3 / 2 * r
    return x, y

# POINTY-TOP hex corners (note the -30° offset)
def hex_corners(cx, cy, size):
    corners = []
    for i in range(6):
        angle = math.radians(60 * i - 30)
        corners.append((
            cx + size * math.cos(angle),
            cy + size * math.sin(angle)
        ))
    return corners

# Filled hex "circle" (radius shape)
def hex_circle(radius):
    results = []
    for q in range(-radius, radius + 1):
        r1 = max(-radius, -q - radius)
        r2 = min(radius, -q + radius)
        for r in range(r1, r2 + 1):
            results.append((q, r))
    return results

# ---------------- MAIN ----------------
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Correctly Aligned Hex Grid")
clock = pygame.time.Clock()

hexes = hex_circle(RADIUS)

running = True
while running:
    clock.tick(60)
    screen.fill(BG_COLOR)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    for q, r in hexes:
        px, py = axial_to_pixel(q, r, HEX_SIZE)
        px += WIDTH // 2
        py += HEIGHT // 2

        corners = hex_corners(px, py, HEX_SIZE - 1)
        pygame.draw.polygon(screen, HEX_COLOR, corners)
        pygame.draw.polygon(screen, OUTLINE_COLOR, corners, 2)

    pygame.display.flip()

pygame.quit()
