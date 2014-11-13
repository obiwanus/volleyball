#include <SDL2/SDL.h>
#include <stdio.h>

// Functions
bool init();
bool loadMedia();
void close();  // Frees media and shuts down SDL


// Vars
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloSDL = NULL;  // The image we'll load


int main(int argc, char const *argv[])
{
    enum {SUCCESS = 0, ERROR = 1};

    if (!init())
        return ERROR;

    if (!loadMedia())
        return ERROR;

    // Apply the image
    SDL_BlitSurface(gHelloSDL, NULL, gScreenSurface, NULL);
    SDL_UpdateWindowSurface(gWindow);

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

    // Get window surface
    gScreenSurface = SDL_GetWindowSurface(gWindow);

    return true;
}


bool loadMedia() 
{
    gHelloSDL = SDL_LoadBMP("ball.png");
    if (gHelloSDL == NULL) {
        printf("Unable to load image: %s\n", SDL_GetError());
        return false;
    }
    return true;
}


void close() {
    // Deallocate surface
    SDL_FreeSurface(gHelloSDL);
    gHelloSDL = NULL;

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    // Quit SDL
    SDL_Quit();
}
