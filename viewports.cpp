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

        SDL_Rect fillRect;

        // Top left viewport
        SDL_Rect topLeftViewport = { 0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
        SDL_RenderSetViewport(gRenderer, &topLeftViewport);
        
        fillRect.x = topLeftViewport.w / 4;
        fillRect.y = topLeftViewport.h / 4;
        fillRect.w = topLeftViewport.w / 2;
        fillRect.h = topLeftViewport.h / 2;
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0x88);
        SDL_RenderFillRect(gRenderer, &fillRect);

        // Top right viewport
        SDL_Rect topRightViewport = {SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
        SDL_RenderSetViewport(gRenderer, &topRightViewport);
        
        fillRect.x = topRightViewport.w / 4;
        fillRect.y = topRightViewport.h / 4;
        fillRect.w = topRightViewport.w / 2;
        fillRect.h = topRightViewport.h / 2;
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0x88);
        SDL_RenderFillRect(gRenderer, &fillRect);

        // Bottom viewport
        SDL_Rect bottomViewport = { 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
        SDL_RenderSetViewport(gRenderer, &bottomViewport);
        
        fillRect.x = bottomViewport.w / 4;
        fillRect.y = bottomViewport.h / 4;
        fillRect.w = bottomViewport.w / 2;
        fillRect.h = bottomViewport.h / 2;
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0x88);
        SDL_RenderFillRect(gRenderer, &fillRect);

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
