#include <SDL/SDL.h>
#include <math.h>

#define W 500 //window width
#define H 500 //window height

static SDL_Surface *surface = NULL;

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

    static double theta = 0;
    theta+=0.01;
    square(250, 250+150*cos(theta), 250, 250+150*sin(theta), 0x885555);
    line(250, 250+150*cos(theta), 250, 250+150*sin(theta), 0x00FFFF);

}

int main() 
{
    surface = SDL_SetVideoMode(W, H, 32, 0);
    int exit_loop = 0;
    for (;;) {
        SDL_LockSurface(surface);
        DrawScreen();
        SDL_UnlockSurface(surface);
        SDL_Flip(surface);

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch(ev.type) {
                case SDL_QUIT:
                    exit_loop = 1;
            }
        }
        if (exit_loop) break;
        SDL_Delay(10);
    }
    SDL_Quit();
    return 0;
}
