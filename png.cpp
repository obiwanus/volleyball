#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>


// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();  

// Loads individual image
SDL_Surface* loadSurface(std::string path);  


// Vars
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 860;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

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

    // Apply the current image stretched
    SDL_Rect stretchRect;
    stretchRect.x = 0;
    stretchRect.y = 0;
    stretchRect.w = SCREEN_WIDTH;
    stretchRect.h = SCREEN_HEIGHT;
    SDL_BlitScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect);
    SDL_UpdateWindowSurface(gWindow);

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                continue;
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

    // Init PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_Image could not initialize. %s\n", IMG_GetError());
        return false;
    }

    // Get window surface
    gScreenSurface = SDL_GetWindowSurface(gWindow);

    return true;
}


bool loadMedia() 
{
    gCurrentSurface = loadSurface("ball.png");
    return true;
}


SDL_Surface* loadSurface(std::string path)
{
    // The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s: %s\n", path.c_str(), IMG_GetError());
        return NULL;
    }
    // Convert surface to screen format
    optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);

    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);

    return optimizedSurface;
}


void close() 
{
    SDL_FreeSurface(gCurrentSurface);
    SDL_FreeSurface(gScreenSurface);

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    // Quit SDL
    SDL_Quit();
}
