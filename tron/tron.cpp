#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <clocale>
#include <unistd.h>

constexpr int TICK_US   = 55000;  // ~18 fps, snappy but readable
constexpr int MIN_W     = 30;
constexpr int MIN_H     = 16;

// color pairs
enum Colors { C_WALL=1, C_P1, C_P1_HEAD, C_P2, C_P2_HEAD, C_HUD, C_TITLE };
enum Dir    { UP, DOWN, LEFT, RIGHT };
enum Cell   { EMPTY, WALL, TRAIL_1, TRAIL_2 };

struct Player {
    int x, y, dir, alive, score;
};

// unicode trail segments: vert, horiz, and 4 corners
static const char *seg_v  = "│";
static const char *seg_h  = "─";
static const char *seg_ul = "╭";
static const char *seg_ur = "╮";
static const char *seg_dl = "╰";
static const char *seg_dr = "╯";
static const char *seg_hd = "●";

static int W, H;
static Cell *grid;
static int  *prev_dir;  // previous direction per cell (for corner detection)
static Player p1, p2;

static inline int idx(int x, int y) { return y * W + x; }
static inline bool blocked(int x, int y) {
    return x<=0 || x>=W-1 || y<=0 || y>=H-1 || grid[idx(x,y)]!=EMPTY;
}

static void colors_init() {
    start_color();
    use_default_colors();
    init_pair(C_WALL,    COLOR_WHITE,   -1);
    init_pair(C_P1,      COLOR_CYAN,    -1);
    init_pair(C_P1_HEAD, COLOR_WHITE,   COLOR_CYAN);
    init_pair(C_P2,      COLOR_MAGENTA, -1);
    init_pair(C_P2_HEAD, COLOR_WHITE,   COLOR_MAGENTA);
    init_pair(C_HUD,     COLOR_YELLOW,  -1);
    init_pair(C_TITLE,   COLOR_CYAN,    -1);
}

static void draw_border() {
    attron(COLOR_PAIR(C_WALL) | A_DIM);
    for (int x=0; x<W; x++) { mvaddstr(0,x,"─"); mvaddstr(H-1,x,"─"); }
    for (int y=0; y<H; y++) { mvaddstr(y,0,"│"); mvaddstr(y,W-1,"│"); }
    mvaddstr(0,   0,   "╭"); mvaddstr(0,   W-1, "╮");
    mvaddstr(H-1, 0,   "╰"); mvaddstr(H-1, W-1, "╯");
    attroff(COLOR_PAIR(C_WALL) | A_DIM);
}

static void grid_init() {
    std::memset(grid, EMPTY, W*H*sizeof(Cell));
    std::memset(prev_dir, -1, W*H*sizeof(int));
    for (int x=0;x<W;x++) { grid[idx(x,0)]=WALL; grid[idx(x,H-1)]=WALL; }
    for (int y=0;y<H;y++) { grid[idx(0,y)]=WALL; grid[idx(W-1,y)]=WALL; }
}

static void players_init() {
    p1 = {W/4,     H/2, RIGHT, 1, p1.score};
    p2 = {3*W/4,   H/2, LEFT,  1, p2.score};
    grid[idx(p1.x,p1.y)] = TRAIL_1;
    grid[idx(p2.x,p2.y)] = TRAIL_2;
    prev_dir[idx(p1.x,p1.y)] = p1.dir;
    prev_dir[idx(p2.x,p2.y)] = p2.dir;
}

// pick the right trail char based on direction change
static const char* trail_char(int old_dir, int new_dir) {
    if (old_dir == new_dir || old_dir < 0) {
        return (new_dir==UP||new_dir==DOWN) ? seg_v : seg_h;
    }
    // corners
    if ((old_dir==UP    && new_dir==RIGHT)||(old_dir==LEFT  && new_dir==DOWN))  return seg_ul;
    if ((old_dir==UP    && new_dir==LEFT) ||(old_dir==RIGHT && new_dir==DOWN))  return seg_ur;
    if ((old_dir==DOWN  && new_dir==RIGHT)||(old_dir==LEFT  && new_dir==UP))    return seg_dl;
    if ((old_dir==DOWN  && new_dir==LEFT) ||(old_dir==RIGHT && new_dir==UP))    return seg_dr;
    return (new_dir==UP||new_dir==DOWN) ? seg_v : seg_h;
}

static void draw_trail(Player &p, int old_dir, int cp, int cp_head, Cell cell_type) {
    // replace old head with trail segment
    const char *tc = trail_char(
        prev_dir[idx(p.x - (p.dir==RIGHT?1:p.dir==LEFT?-1:0),
                      p.y - (p.dir==DOWN?1:p.dir==UP?-1:0))],
        p.dir
    );
    int ox = p.x - (p.dir==RIGHT?1:p.dir==LEFT?-1:0);
    int oy = p.y - (p.dir==DOWN ?1:p.dir==UP  ?-1:0);
    if (ox>0 && ox<W-1 && oy>0 && oy<H-1) {
        attron(COLOR_PAIR(cp) | A_BOLD);
        mvaddstr(oy, ox, tc);
        attroff(COLOR_PAIR(cp) | A_BOLD);
    }
    // draw new head
    attron(COLOR_PAIR(cp_head) | A_BOLD);
    mvaddstr(p.y, p.x, seg_hd);
    attroff(COLOR_PAIR(cp_head) | A_BOLD);
}

static void move_player(Player &p, Cell trail, int cp, int cp_head) {
    if (!p.alive) return;
    int nx=p.x, ny=p.y;
    switch(p.dir) {
        case UP:    ny--; break;
        case DOWN:  ny++; break;
        case LEFT:  nx--; break;
        case RIGHT: nx++; break;
    }
    if (blocked(nx,ny)) { p.alive=0; return; }
    int old_dir = prev_dir[idx(p.x,p.y)];
    p.x=nx; p.y=ny;
    grid[idx(nx,ny)] = trail;
    prev_dir[idx(nx,ny)] = p.dir;
    draw_trail(p, old_dir, cp, cp_head, trail);
}

// AI: avoid immediate death, prefer current direction, slight randomness
static void ai_think(Player &ai) {
    int dx[]={0,0,-1,1}, dy[]={-1,1,0,0};
    int opposite[]={DOWN,UP,RIGHT,LEFT};

    // current dir safe? keep going most of the time
    int nx=ai.x+dx[ai.dir], ny=ai.y+dy[ai.dir];
    if (!blocked(nx,ny) && (rand()%100 < 75)) return;

    // find safe directions
    int safe[4], n=0;
    for (int d=0; d<4; d++) {
        if (d==opposite[ai.dir]) continue;
        if (!blocked(ai.x+dx[d], ai.y+dy[d])) safe[n++]=d;
    }
    if (n==0) { // desperation: try opposite
        if (!blocked(ai.x+dx[opposite[ai.dir]], ai.y+dy[opposite[ai.dir]]))
            ai.dir = opposite[ai.dir];
        return; // dead next tick anyway
    }

    // prefer direction with more open space (lookahead 5 cells)
    int best=safe[0], best_space=0;
    for (int i=0; i<n; i++) {
        int space=0;
        int cx=ai.x, cy=ai.y;
        for (int s=0; s<5; s++) {
            cx+=dx[safe[i]]; cy+=dy[safe[i]];
            if (blocked(cx,cy)) break;
            space++;
        }
        if (space>best_space) { best_space=space; best=safe[i]; }
    }
    ai.dir = best;
}

static void draw_hud() {
    attron(COLOR_PAIR(C_HUD));
    mvprintw(H, 0, " YOU %d  —  AI %d   [WASD/Arrows] Move  [R] Restart  [Q] Quit ",
             p1.score, p2.score);
    attroff(COLOR_PAIR(C_HUD));
}

static const char *title[] = {
    "  ▀▀█▀▀ █▀▀█ █▀▀█ █▀▀▄ ",
    "    █   █▄▄▀ █  █ █  █ ",
    "    █   █  █ █▄▄█ █  █ ",
    nullptr
};

static void draw_title() {
    int tw=25;
    int sx=(W-tw)/2, sy=H/2-5;
    attron(COLOR_PAIR(C_TITLE) | A_BOLD);
    for (int i=0; title[i]; i++)
        mvaddstr(sy+i, sx, title[i]);
    attroff(COLOR_PAIR(C_TITLE) | A_BOLD);
    attron(COLOR_PAIR(C_HUD));
    const char *sub = "[ Press any key to start ]";
    mvaddstr(sy+5, (W-(int)strlen(sub))/2, sub);
    attroff(COLOR_PAIR(C_HUD));
}

static void show_result(const char *msg) {
    int len=strlen(msg);
    attron(COLOR_PAIR(C_HUD) | A_BOLD);
    mvaddstr(H/2, (W-len)/2, msg);
    attroff(COLOR_PAIR(C_HUD) | A_BOLD);
    attron(COLOR_PAIR(C_HUD));
    const char *sub = "[ R to replay | Q to quit ]";
    mvaddstr(H/2+2, (W-(int)strlen(sub))/2, sub);
    attroff(COLOR_PAIR(C_HUD));
    refresh();
}

static int handle_input() {
    int ch = getch();
    if (ch=='q'||ch=='Q') return -1;
    if (ch=='r'||ch=='R') return 1;
    int opposite[]={DOWN,UP,RIGHT,LEFT};
    int new_dir = -1;
    switch(ch) {
        case 'w': case 'W': case KEY_UP:    new_dir=UP;    break;
        case 's': case 'S': case KEY_DOWN:  new_dir=DOWN;  break;
        case 'a': case 'A': case KEY_LEFT:  new_dir=LEFT;  break;
        case 'd': case 'D': case KEY_RIGHT: new_dir=RIGHT; break;
    }
    if (new_dir>=0 && new_dir!=opposite[p1.dir]) p1.dir=new_dir;
    return 0;
}

int main() {
    srand(time(nullptr));
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(0);
    colors_init();

    // size check
    getmaxyx(stdscr, H, W);
    if (W<MIN_W||H<MIN_H) {
        endwin();
        fprintf(stderr, "Terminal too small. Need %dx%d, got %dx%d\n", MIN_W,MIN_H,W,H);
        return 1;
    }
    H--;  // reserve bottom row for HUD

    grid = new Cell[W*H];
    prev_dir = new int[W*H];

    // title screen
    grid_init();
    draw_border();
    draw_title();
    draw_hud();
    refresh();
    timeout(-1);  // block for keypress
    getch();
    timeout(0);

    bool running = true;
    while (running) {
        // round setup
        grid_init();
        players_init();
        erase();
        draw_border();
        draw_player_initial:
        attron(COLOR_PAIR(C_P1_HEAD)|A_BOLD);
        mvaddstr(p1.y, p1.x, seg_hd);
        attroff(COLOR_PAIR(C_P1_HEAD)|A_BOLD);
        attron(COLOR_PAIR(C_P2_HEAD)|A_BOLD);
        mvaddstr(p2.y, p2.x, seg_hd);
        attroff(COLOR_PAIR(C_P2_HEAD)|A_BOLD);
        draw_hud();
        refresh();

        // countdown
        for (int i=3; i>0; i--) {
            attron(COLOR_PAIR(C_HUD)|A_BOLD);
            mvprintw(H/2, W/2-1, " %d ", i);
            attroff(COLOR_PAIR(C_HUD)|A_BOLD);
            refresh();
            napms(600);
        }
        attron(COLOR_PAIR(C_HUD)|A_BOLD);
        mvaddstr(H/2, W/2-2, " GO! ");
        attroff(COLOR_PAIR(C_HUD)|A_BOLD);
        refresh();
        napms(300);
        // clear the countdown text
        mvaddstr(H/2, W/2-2, "     ");

        // game loop
        bool round_over = false;
        while (!round_over) {
            int inp = handle_input();
            if (inp==-1) { running=false; break; }
            if (inp==1)  break;

            ai_think(p2);
            move_player(p1, TRAIL_1, C_P1, C_P1_HEAD);
            move_player(p2, TRAIL_2, C_P2, C_P2_HEAD);

            if (!p1.alive || !p2.alive) {
                if (!p1.alive && !p2.alive)      show_result("  DRAW!  ");
                else if (!p1.alive) { p2.score++; show_result("  AI WINS!  "); }
                else                { p1.score++; show_result("  YOU WIN!  "); }
                draw_hud();
                refresh();

                // wait for R or Q
                timeout(-1);
                while (true) {
                    int ch=getch();
                    if (ch=='q'||ch=='Q') { running=false; break; }
                    if (ch=='r'||ch=='R') break;
                }
                timeout(0);
                round_over = true;
            }

            draw_hud();
            refresh();
            usleep(TICK_US);
        }
    }

    delete[] grid;
    delete[] prev_dir;
    endwin();
    return 0;
}
