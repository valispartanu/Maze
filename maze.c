#define SDL_MAIN_HANDLED
#include "maze.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (1000)
#define WINDOW_HEIGHT (800)

// speed in pixels/second
#define SPEED (400)

struct position {
    float x_pos;
    float y_pos;
};

struct velocity {
    int x_vel;
    int y_vel;
};

struct move {
    int up;
    int down;
    int left;
    int right;
};

static void fail() {
    fprintf(stderr, "Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}
static int I(int n) {
    if(n<0) fail();
    return n;
}
static void *P(void *p) {
    if (p==NULL) fail();
    return p;
}

//get input from keyboard
void control (SDL_Event event, move *m) {
    switch (event.key.keysym.sym) {
    case SDLK_w:
    case SDLK_UP:
        if(event.type == SDL_KEYDOWN) m->up = 1;
        else if(event.type == SDL_KEYUP) m->up = 0;
        break;
    case SDLK_a:
    case SDLK_LEFT:
        if(event.type == SDL_KEYDOWN) m->left = 1;
        else if(event.type == SDL_KEYUP) m->left = 0;
        break;
    case SDLK_s:
    case SDLK_DOWN:
        if(event.type == SDL_KEYDOWN) m->down = 1;
        else if(event.type == SDL_KEYUP) m->down = 0;
        break;
    case SDLK_d:
    case SDLK_RIGHT:
        if(event.type == SDL_KEYDOWN) m->right = 1;
        else if(event.type == SDL_KEYUP) m->right = 0;
        break;
    }
}

//determine velocity
void det_velocity(velocity *v, move *m){
    if ((m->up) && !(m->down)) v->y_vel = -SPEED;
    if ((m->down) && !(m->up)) v->y_vel = SPEED;
    if ((m->left) && !(m->right)) v->x_vel = -SPEED;
    if ((m->right) && !(m->left)) v->x_vel = SPEED;
}

//update positions
void update_pos(position *p, velocity *v){
    p->x_pos += v->x_vel / 60;
    p->y_pos += v->y_vel / 60;
}

//collision detection with bounds
void collision(position *p, SDL_Rect loc){
    if (p->x_pos <= 0) p->x_pos = 0;
    if (p->y_pos <= 0) p->y_pos = 0;
    if (p->x_pos >= WINDOW_WIDTH - loc.w) p->x_pos = WINDOW_WIDTH - loc.w;
    if (p->y_pos >= WINDOW_HEIGHT - loc.h) p->y_pos = WINDOW_HEIGHT - loc.h;
}

//set the colour and fill the maze with that colour
void create_wall(SDL_Rect t, SDL_Renderer *r) {
    I(SDL_SetRenderDrawColor(r, 0, 0, 255, 155));
    I(SDL_RenderFillRect(r, &t));
}

//read maze coordinates from a file
void read_file(SDL_Renderer *r) {
    const int max = 20;
    char line[max];
    SDL_Rect t;
    FILE *in = fopen("wall.txt", "r");
    fgets(line, max, in);
    while (! feof(in)) {
        sscanf(line, "%d %d %d %d", &t.x, &t.y, &t.w, &t.h);
        create_wall(t, r);
        fgets(line, max, in);
    }
    fclose(in);
}

//check game over
void game_over(SDL_Renderer *r){
    SDL_Surface *surface = P(IMG_Load("Game_Over.jpg"));
    SDL_Texture *texture = P(SDL_CreateTextureFromSurface(r, surface));
    SDL_RenderCopy(r, texture, NULL, NULL);
    SDL_RenderPresent(r);
    SDL_Delay(3000);
    SDL_DestroyRenderer(r);
    SDL_Quit();
}

//check if the sprite reached the end of maze
void check_finish(position *p, SDL_Renderer *r){
    if(p->x_pos == 0 && p->y_pos >=700) game_over(r);
}

int main()
{
    I(SDL_Init(SDL_INIT_EVERYTHING| SDL_INIT_AUDIO));

    SDL_Window *win = P(SDL_CreateWindow("Sky",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT,0));

    // create a renderer, which sets up the graphics hardware
    SDL_Renderer *rend = P(SDL_CreateRenderer(win, -1, 0));

    //background
    SDL_Surface *b_surface = P(IMG_Load("space.bmp"));
    SDL_Texture *b_texture = P(SDL_CreateTextureFromSurface(rend, b_surface));

    //load the image into memory using SDL_image library function
    SDL_Surface *surface = P(IMG_Load("sun.png"));
    SDL_Texture *tex = P(SDL_CreateTextureFromSurface(rend, surface));
    SDL_FreeSurface(surface);

    // struct to hold the position and size of the sprite
    SDL_Rect loc;

    // get and scale the dimensions of texture
    SDL_QueryTexture(tex, NULL, NULL, &loc.w, &loc.h);
    loc.w /= 4;
    loc.h /= 4;

    // start sprite at the beginning of the maze
    position *p = malloc(sizeof(position));
    p->x_pos = 820;
    p->y_pos = 750;

    velocity *v = malloc(sizeof(velocity));
    v->x_vel = 0;
    v->y_vel = 0;

    // keep track of which inputs are given
    move *m = malloc(sizeof(move));
    m->up = 0;
    m->down = 0;
    m->left = 0;
    m->right = 0;

    // set to 1 when window close button is pressed
    int close_requested = 0;

    // animation loop
    while (!close_requested)
    {
        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                close_requested = 1;
                break;
            case SDL_KEYDOWN:
                control(event, m);
                break;
            case SDL_KEYUP:
                control(event, m);
                break;
            }
            break;
        }

        v->x_vel = 0;
        v->y_vel = 0;

        det_velocity(v, m);
        update_pos(p, v);
        collision(p, loc);

        // set the positions in the struct
        loc.y = (int) p->y_pos;
        loc.x = (int) p->x_pos;

        // draw the image to the window
        SDL_RenderCopy(rend, b_texture, NULL, NULL);
        SDL_RenderCopy(rend, tex, NULL, &loc);
        read_file(rend);
        check_finish(p, rend);
        /*if((rend.x+rend.w) >= (p->x_pos) && (rend.x) <= ((p->x_pos) + loc.w) && (rend.y + rend.h) >= (p->y_pos) && (rend.y) <= (p->y_pos + loc.h)){
        close_requested = 1;}*/
        if((915 >= p->x_pos) && (515 <= p->x_pos + loc.w) && (545 >= p->y_pos) && (515 <= p->y_pos + loc.h)){
        close_requested = 1;}
        if((545 >= p->x_pos) && (515 <= p->x_pos + loc.w) && (615 >= p->y_pos) && (515 <= p->y_pos + loc.h)){
        close_requested = 1;}
        if((515 >= p->x_pos) && (415 <= p->x_pos + loc.w) && (615 >= p->y_pos) && (585 <= p->y_pos + loc.h)){
        close_requested = 1;}
        if((615 >= p->x_pos) && (315 <= p->x_pos + loc.w) && (715 >= p->y_pos) && (685 <= p->y_pos + loc.h)){
        close_requested = 1;}
        if((330 >= p->x_pos) && (300 <= p->x_pos + loc.w) && (715 >= p->y_pos) && (315 <= p->y_pos + loc.h)){
        close_requested = 1;}
        SDL_RenderPresent(rend);

        // wait 1/60th of a second
        SDL_Delay(1000/60);
    }

    return 0;
}
