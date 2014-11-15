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

// Loads individual image as a texture
SDL_Texture* loadTexture(std::string path);  


// Vars
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 860;

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

    if (!loadMedia())
        return ERROR;

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                continue;
            }
        }

        // Clear screen
        SDL_RenderClear(gRenderer);

        // Render texture to screen
        SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

        // Update screen
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

    // Init renderer color
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Init PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_Image could not initialize. %s\n", IMG_GetError());
        return false;
    }

    return true;
}


bool loadMedia() 
{
    gTexture = loadTexture("texture.png");
    if (gTexture == NULL) {
        printf("Failed to load texture image.\n");
        return false;
    }
    return true;
}


SDL_Texture* loadTexture(std::string path)
{
    // The final texture
    SDL_Texture* newTexture = NULL;

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s: %s\n", path.c_str(), IMG_GetError());
        return NULL;
    }
    
    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    
    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);

    return newTexture;
}


void close() 
{
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    // Quit SDL
    IMG_Quit();
    SDL_Quit();
}
