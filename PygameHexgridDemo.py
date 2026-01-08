import pygame, math
pygame.init()
W,H=800,600; screen=pygame.display.set_mode((W,H)); clock=pygame.time.Clock()
s=30; SQ=math.sqrt(3)

def a2p(q,r):  # axial->pixel (pointy)
    return SQ*s*(q+r/2), 1.5*s*r

def hexpts(cx,cy):
    return [(cx+s*math.cos(math.radians(60*i-30)),
             cy+s*math.sin(math.radians(60*i-30))) for i in range(6)]

# camera in fractional axial coords (wrapped => perfect loop)
cam_q=cam_r=0.0
P=40  # repeat period in hexes (bigger = less obvious repetition)

# screen-sized draw window (+margin)
Q=int(W/(SQ*s))+3
R=int(H/(1.5*s))+3

# pan speed in pixels/sec (diagonal)
vx,vy=90.0,60.0

run=True
while run:
    dt=clock.tick(60)/1000
    for e in pygame.event.get():
        if e.type==pygame.QUIT: run=False

    # pixel velocity -> axial delta (inverse of a2p)
    dq=(SQ/3*vx - 1/3*vy)/s * dt
    dr=(2/3*vy)/s * dt
    cam_q=(cam_q+dq)%P
    cam_r=(cam_r+dr)%P

    screen.fill((20,20,20))
    cq,cr=int(cam_q),int(cam_r)   # draw around camera’s current cell
    for q in range(cq-Q, cq+Q+1):
        for r in range(cr-R, cr+R+1):
            x,y=a2p((q-cam_q), (r-cam_r))   # RELATIVE coords = camera built-in
            x+=W/2; y+=H/2
            if x<-2*s or x>W+2*s or y<-2*s or y>H+2*s: continue
            pygame.draw.polygon(screen,(120,200,255),hexpts(x,y),1)

    pygame.display.flip()
pygame.quit()
