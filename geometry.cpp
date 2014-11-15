#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>


// Starts up SDL and creates window
bool init();

// Frees media and shuts down SDL
void close();  


// Vars
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// The window we'll be rendering to
SDL_Window* gWindow = NULL;

// The window renderer
SDL_Renderer* gRenderer = NULL;

// Currently displayed texture
SDL_Texture* gTexture = NULL;


int main(int argc, char const *argv[])
{
    enum {SUCCESS = 0, ERROR = 1};

    if (!init())
        return ERROR;

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                continue;
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);
        
        SDL_Rect fillRect = {
            SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4,
            SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2
        };
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0x88);
        SDL_RenderFillRect(gRenderer, &fillRect);

        SDL_Rect outlineRect = {
            SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6,
            SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3
        };
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderDrawRect(gRenderer, &outlineRect);

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);

        SDL_SetRenderDrawColor(gRenderer, 0xAA, 0xAA, 0x00, 0xFF);
        for (int i = 2; i < SCREEN_HEIGHT; i += 4) {
            SDL_RenderDrawPoint(gRenderer, SCREEN_WIDTH / 2, i);
        }

        SDL_RenderPresent(gRenderer);
    }

    close();

    return SUCCESS;
}


bool init() 
{
    // Init
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize. Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow(
        "Hello SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN
    );
    if (gWindow == NULL) {
        printf("Window could not be created. Error: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL) {
        printf("Renderer could not be created. Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}


void close() 
{
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    // Quit SDL
    SDL_Quit();
}
