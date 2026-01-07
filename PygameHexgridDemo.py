import pygame, math

pygame.init()
W, H = 800, 600
screen = pygame.display.set_mode((W, H))
clock = pygame.time.Clock()

size = 30

def axial_to_pixel(q, r, s):
    x = math.sqrt(3) * s * (q + r/2)
    y = 1.5 * s * r
    return x, y

def hex_points(cx, cy, s):
    pts = []
    for i in range(6):
        a = math.radians(60*i - 30)
        pts.append((cx + s*math.cos(a), cy + s*math.sin(a)))
    return pts

# camera in world pixels
cam_x, cam_y = 0.0, 0.0
vx, vy = 60.0, 0.0  # pixels/second (pan right)

# how many hex coords to draw around the camera (overshoot is fine)
Q_RANGE = 25
R_RANGE = 25

running = True
while running:
    dt = clock.tick(60) / 1000.0  # seconds
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            running = False

    # move camera (this is the panning)
    cam_x += vx * dt
    cam_y += vy * dt

    screen.fill((20, 20, 20))

    # pick an approximate "center hex" to loop around
    # (simple approach: just loop a fixed box; good enough for now)
    for q in range(-Q_RANGE, Q_RANGE + 1):
        for r in range(-R_RANGE, R_RANGE + 1):
            wx, wy = axial_to_pixel(q, r, size)      # world coords
            sx = wx - cam_x + W/2                    # to screen
            sy = wy - cam_y + H/2

            # quick cull: skip drawing if it's far off-screen
            if sx < -2*size or sx > W + 2*size or sy < -2*size or sy > H + 2*size:
                continue

            pygame.draw.polygon(screen, (120, 200, 255), hex_points(sx, sy, size), 1)

    pygame.display.flip()

pygame.quit()
