#include <SDL/SDL.h>
#include <math.h>

#define W 500 //window width
#define H 500 //window height

static SDL_Surface *surface = NULL;

double p_x = W/2; //player x
double p_y = H/2; //player y
double p_t = 0; //player theta
double p_v = 3; //player velocity

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
        pix[W*(int)y+(int)x] = color; //cast to int for cheap floor() 
        x+=u_x;
        y+=u_y;
    }
}

void hline(int x1, int x2, int y, int color)
{
    int *pix = (int*) surface->pixels;
    if (x1 > x2) {
        int swap = x1;
        x1 = x2;
        x2 = swap;
    }

    for(int x=x1; x<=x2; ++x){
        pix[y*W+x] = color;
    }
}

void vline(int x, int y1, int y2, int color)
{
    int *pix = (int*) surface->pixels;

    if (y1 > y2) {
        int swap = y1;
        y1 = y2;
        y2 = swap;
    }

    for(int y=y1; y<=y2; ++y){
        pix[y*W+x] = color;
    }
}

void square(int x1, int x2, int y1, int y2, int color)
{
    hline(x1, x2, y1, color);
    hline(x1, x2, y2, color);
    vline(x1, y1, y2, color);
    vline(x2, y1, y2, color);
}

void ClearScreen() {
    int *pix = (int*) surface->pixels;
    for (int i=0; i<H*W; ++i) {
        pix[i] = 0x000000;
    }
}

void DrawScreen() {
    ClearScreen();
    vline(10, 10, 20, 0xFFFFFF);
    vline(30, 60, 200, 0xFF0000);
    hline(150, 200, 100, 0xFF00FF);
    hline(200, 400, 300, 0x0000FF);

    square(p_x - 5, p_x + 5, p_y - 5, p_y + 5, 0xFFFFFF);
    line(p_x, p_x + 10*cos(p_t), p_y, p_y + 10*sin(p_t), 0x00FF00);

}

int main() 
{
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
    return 0;
}
