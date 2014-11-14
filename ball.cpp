#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>


// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();  

// Loads individual image
SDL_Surface* loadSurface(std::string path);  


// Vars
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Key press surfaces constants
enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

// The images that correspond to a keypress
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

// Currently displayed image
SDL_Surface* gCurrentSurface = NULL;


int main(int argc, char const *argv[])
{
    enum {SUCCESS = 0, ERROR = 1};

    if (!init())
        return ERROR;

    if (!loadMedia())
        return ERROR;

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Set default surface
    gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

    // Apply the current image
    SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
    SDL_UpdateWindowSurface(gWindow);

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                continue;
            }
            if (e.type == SDL_KEYDOWN) {
                // Select surfaces based on the key press
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
                        break;
                    case SDLK_DOWN:
                        gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
                        break;
                    case SDLK_LEFT:
                        gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
                        break;
                    case SDLK_RIGHT:
                        gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
                        break;
                    default:
                        gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
                        break;
                }
                // Apply the current image
                SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
                SDL_UpdateWindowSurface(gWindow);
            }
        }

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

    // Get window surface
    gScreenSurface = SDL_GetWindowSurface(gWindow);

    return true;
}


bool loadMedia() 
{
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("04_key_presses/press.bmp");
    gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("04_key_presses/up.bmp");
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("04_key_presses/down.bmp");
    gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("04_key_presses/left.bmp");
    gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("04_key_presses/right.bmp");
    return true;
}


SDL_Surface* loadSurface(std::string path)
{
    SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s: %s\n", path.c_str(), SDL_GetError());
    }
    return loadedSurface;
}


void close() 
{
    for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; i++)
        SDL_FreeSurface(gKeyPressSurfaces[i]);

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    // Quit SDL
    SDL_Quit();
}
