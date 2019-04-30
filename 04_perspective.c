#include <SDL/SDL.h>
#include <math.h>

#define W 500 //window width
#define H 500 //window height
#define hfov (100*H) //TODO get a better intuitive understanding of this

static SDL_Surface *surface = NULL;

double p_x = W/2; //player x
double p_y = H/2; //player y
double p_t = 0; //player theta
const double p_v = 3; //player velocity

// for now we are making 2D walls which are just lines
static struct wall {
    int x1;
    int x2;
    int y1;
    int y2;
    int color;
} *walls = NULL;
static int num_walls = 0;


// some terrible line drawing algorithm i just came up with
void line(int x1, int x2, int y1, int y2, int color)
{
    int *pix = (int*) surface->pixels;

    // create unit vector
    double len = sqrt(pow(y2-y1,2)+pow(x2-x1,2));
    double u_x = (x2 - x1) / len;
    double u_y = (y2 - y1) / len;

    double x = x1;
    double y = y1;
    
    // take approx len steps
    for(int i=0; i<len; ++i) {
        //refuse to draw off the screen (risk of segfault)
        if (y >= 0 && y < H && x >= 0 && x < W) {
            pix[W*(int)y+(int)x] = color; //cast to int for cheap floor() 
        }
        x+=u_x;
        y+=u_y;
    }
}

void square(int x1, int x2, int y1, int y2, int color)
{
    line(x1, x2, y1, y1, color);
    line(x1, x2, y2, y2, color);
    line(x1, x1, y1, y2, color);
    line(x2, x2, y1, y2, color);
}

void ClearScreen() {
    int *pix = (int*) surface->pixels;
    for (int i=0; i<H*W; ++i) {
        pix[i] = 0x000000;
    }
}

double intersectX(double x1, double x2, double y1, double y2, double x3, double x4, double y3, double y4) {
    // find the x coordinate of two intersecting lines
    // using two lines as defined by points and intersection formula
    double num = (x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4 - y3*x4);
    double denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    return num / denom;
}
double intersectY(double x1, double x2, double y1, double y2, double x3, double x4, double y3, double y4) {
    // find the x coordinate of two intersecting lines
    // using two lines as defined by points and intersection formula
    double num = (x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4 - y3*x4);
    double denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    return num / denom;
}

void DrawScreen() {
    ClearScreen();

    // calculate origin
    int o_x = W/2;
    int o_y = H/2;

    // draw walls
    for(int i=0; i<num_walls; ++i) {
        struct wall w = walls[i];

        // find coordinates in terms of their offset from player
        double x1_a = w.x1 - p_x;
        double y1_a = w.y1 - p_y;
        double x2_a = w.x2 - p_x;
        double y2_a = w.y2 - p_y;

        // apply rotation matrix cross product 
        // (a positive rotation of the player corresponse
        // to a negative rotation of the world)
        double x1_b = x1_a*cos(-p_t) - y1_a*sin(-p_t);
        double y1_b = x1_a*sin(-p_t) + y1_a*cos(-p_t);
        double x2_b = x2_a*cos(-p_t) - y2_a*sin(-p_t);
        double y2_b = x2_a*sin(-p_t) + y2_a*cos(-p_t);

        // don't draw lines that are behind the player
        if (x1_b <= 0 && x2_b <= 0) continue;

        // if one endpoint is behind the player, clip it to frustrum
        if (x1_b <= 0 || x2_b <= 0) {
            x1_b = intersectX(x1_b, x2_b, y1_b, y2_b, 0, 100, 0, 100);
            y1_b = intersectY(x1_b, x2_b, y1_b, y2_b, 0, 100, 0, 100);
            x2_b = intersectX(x1_b, x2_b, y1_b, y2_b, 0, 100, 0, -100);
            y2_b = intersectY(x1_b, x2_b, y1_b, y2_b, 0, 100, 0, -100);
        }
        
        //scale a line based on it's distance (now represented by x coordinate) from the player
#if 0
        float s_1 = hfov / x1_r;
        float s_2 = hfov / x2_r;

        int x1 = (W/2) - (int)(s_1 * y1_r);
        int x2 = (W/2) - (int)(s_2 * y2_r);
        printf("x1_r = %f, x2_r = %f, s1 = %f, s2 = %f, x1 = %d, x2 = %d\n", x1_r, x2_r, s_1, s_2, x1, x2);
#endif 

        // apply offset of origin in top-left corner
        double x1 = x1_b + o_x;
        double y1 = y1_b + o_y;
        double x2 = x2_b + o_x;
        double y2 = y2_b + o_y;
        //line(x1, x2, H/2, H/2, w.color);
        line(x1, x2, y1, y2, w.color);
    }

    // DEBUG draw the 2d frustrum 
    line(o_x, o_x+100, o_y, o_y+100, 0x555555);
    line(o_x, o_x+100, o_y, o_y-100, 0x555555);


    // draw player (at the origin)
    square(o_x - 5, o_x + 5, o_y - 5, o_y + 5, 0xFFFFFF);
    line(o_x, o_x+10, o_y, o_y, 0x00FF00);

}

void init()
{
    // should replace this with something that reads map data from a file
    walls = malloc(5*sizeof(*walls));
    num_walls = 5;
    walls[0].x1 = 50;
    walls[0].x2 = 450;
    walls[0].y1 = 450;
    walls[0].y2 = 450;
    walls[0].color = 0xFF0000;
    walls[1].x1 = 450;
    walls[1].x2 = 450;
    walls[1].y1 = 100;
    walls[1].y2 = 450;
    walls[1].color = 0xFF00FF;
    walls[2].x1 = 250;
    walls[2].x2 = 450;
    walls[2].y1 = 50;
    walls[2].y2 = 100;
    walls[2].color = 0xFFFF00;
    walls[3].x1 = 250;
    walls[3].x2 = 50;
    walls[3].y1 = 50;
    walls[3].y2 = 100;
    walls[3].color = 0x00FF00;
    walls[4].x1 = 50;
    walls[4].x2 = 50;
    walls[4].y1 = 100;
    walls[4].y2 = 450;
    walls[4].color = 0x00FFFF;
}

void deinit()
{
    free(walls);
}

int main() 
{
    init();
    surface = SDL_SetVideoMode(W, H, 32, 0);
    SDL_EnableKeyRepeat(150, 30);
    SDL_ShowCursor(SDL_DISABLE);
    int loop = 1;
    int wasd[4]={0,0,0,0}; //which wasd keys are pressed
    while (loop) {
        SDL_LockSurface(surface);
        DrawScreen();
        SDL_UnlockSurface(surface);
        SDL_Flip(surface);

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch(ev.type) {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    switch(ev.key.keysym.sym) {
                        case 'w': wasd[0] = ev.type==SDL_KEYDOWN; break;
                        case 'a': wasd[1] = ev.type==SDL_KEYDOWN; break;
                        case 's': wasd[2] = ev.type==SDL_KEYDOWN; break;
                        case 'd': wasd[3] = ev.type==SDL_KEYDOWN; break;
                    }
                    break;
                case SDL_QUIT:
                    loop = 0;
            }
        }

        // handle keypresses
        if (wasd[0]) {
            p_x+=p_v*cos(p_t);
            p_y+=p_v*sin(p_t);
        }
        if (wasd[1]) {
            p_t-=0.05;
        }
        if (wasd[2]) {
            p_x-=p_v*cos(p_t);
            p_y-=p_v*sin(p_t);
        }
        if (wasd[3]) {
            p_t+=0.05;
        }
        SDL_Delay(10);
    }
    SDL_Quit();
    deinit();
    return 0;
}
