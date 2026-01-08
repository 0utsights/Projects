import pygame, math

pygame.init()
W, H = 800, 600
screen = pygame.display.set_mode((W, H))
clock = pygame.time.Clock()

size = 30  # outer radius

def axial_to_pixel(q, r, s):
    # pointy-top axial -> pixel (Red Blob)
    x = math.sqrt(3) * s * (q + r/2)
    y = 1.5 * s * r
    return x, y

def pixel_to_axial(x, y, s):
    # inverse of the above (pointy-top)
    q = (math.sqrt(3)/3 * x - 1/3 * y) / s
    r = (2/3 * y) / s
    return q, r

def cube_round(q, r):
    # axial (q,r) -> cube (x,y,z), round, back to axial
    x = q
    z = r
    y = -x - z

    rx, ry, rz = round(x), round(y), round(z)

    dx, dy, dz = abs(rx - x), abs(ry - y), abs(rz - z)
    if dx > dy and dx > dz:
        rx = -ry - rz
    elif dy > dz:
        ry = -rx - rz
    else:
        rz = -rx - ry

    return int(rx), int(rz)  # back to axial q,r

def hex_points(cx, cy, s):
    pts = []
    for i in range(6):
        a = math.radians(60*i - 30)  # pointy-top
        pts.append((cx + s*math.cos(a), cy + s*math.sin(a)))
    return pts

# camera in world pixels
cam_x, cam_y = 0.0, 0.0
vx, vy = 80.0, 50.0  # pixels per second

# how many hexes around camera-center to draw
Q_RANGE = 22
R_RANGE = 22

# optional: wrap in AXIAL space to guarantee "wallpaper repeat"
PERIOD = 80  # bigger = less obvious repetition

running = True
while running:
    dt = clock.tick(60) / 1000.0
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            running = False

    cam_x += vx * dt
    cam_y += vy * dt

    # Find which hex the camera is near (in axial coords)
    fq, fr = pixel_to_axial(cam_x, cam_y, size)
    cq, cr = cube_round(fq, fr)

    # Wrap the *center hex* so the pattern repeats forever
    cq %= PERIOD
    cr %= PERIOD

    screen.fill((20, 20, 20))

    # Draw a window around the camera-center hex
    for dq in range(-Q_RANGE, Q_RANGE + 1):
        for dr in range(-R_RANGE, R_RANGE + 1):
            q = cq + dq
            r = cr + dr

            wx, wy = axial_to_pixel(q, r, size)

            # world -> screen: subtract camera, then center
            sx = wx - cam_x + W/2
            sy = wy - cam_y + H/2

            if sx < -2*size or sx > W + 2*size or sy < -2*size or sy > H + 2*size:
                continue

            pygame.draw.polygon(screen, (120, 200, 255), hex_points(sx, sy, size), 1)

    pygame.display.flip()

pygame.quit()
