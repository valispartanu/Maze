//Module for "maze" game

#include <SDL2/SDL.h>

struct position;
typedef struct position position;

struct velocity;
typedef struct velocity velocity;

struct move;
typedef struct move move;

//get input from keyboard
void control (SDL_Event event, move *m);

//determine velocity
void det_velocity(velocity *v, move *m);

//update positions
void update_pos(position *p, velocity *v);

//collision detection with bounds
void collision(position *p, SDL_Rect loc);

//set the colour and fill the maze with that colour
void create_wall(SDL_Rect t, SDL_Renderer *r);

//read maze coordinates from a file
void read_file(SDL_Renderer *r);

//check game over
void game_over(SDL_Renderer *r);

//check if the sprite reached the end of maze
void check_finish(position *p, SDL_Renderer *r);
