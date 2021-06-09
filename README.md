# Maze
“Maze” is a game written in C. The aim of the game is to reach the end of the maze with the “sun” sprite, without touching the walls. The maze is created by reading a text file with coordinates. This method works fine, but is definitely more costly than the "matrix of tiles" approach.  

Future optimizations can be: including a timer to force the player finish at a certain time, including background music, developing other levels with higher difficulty.  

## Compile command:
gcc -std=c11 -Wall -o maze maze.c -lSDL2 -lSDL2_image -lm

## Run command:
./maze

![](https://github.com/valispartanu/Maze/blob/master/maze.png?raw=true)
