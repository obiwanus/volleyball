#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "ltexture.h"


// Starts up SDL and creates window
bool init();

// Frees media and shuts down SDL
void close();

// Loads media
bool loadMedia();


// Vars
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// The window we'll be rendering to
SDL_Window* gWindow = NULL;

// The window renderer
SDL_Renderer* gRenderer = NULL;

// Walking animation
const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;


void LTexture::render(int x, int y, SDL_Rect* clip)
{
    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    // Set clip rendering dimensions
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    // Render to screen
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}


LTexture::LTexture()
{
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}


LTexture::~LTexture()
{
    free();
}


bool LTexture::loadFromFile(std::string path)
{
    // Get rid of preexisting texture
    free();

    // The final texture
    SDL_Texture* newTexture = NULL;

    // Load image
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image. %s\n", IMG_GetError());
        return false;
    }

    // Color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
        printf("Unable to create texture from %s. %s\n", path.c_str(), SDL_GetError());
        return false;
    }

    // Get image dimensions
    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;

    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);

    mTexture = newTexture;
    return true;
}


void LTexture::free()
{
    if (mTexture == NULL)
        return;

    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}


int main(int argc, char const *argv[])
{
    enum {SUCCESS = 0, ERROR = 1};

    if (!init())
        return ERROR;

    if (!loadMedia())
        return ERROR;

    bool quit = false;
    SDL_Event e;

    // Current animation frame
    int frame = 0;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                continue;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        // Render current frame
        SDL_Rect* currentClip = &gSpriteClips[frame / 4];
        gSpriteSheetTexture.render(
            (SCREEN_WIDTH - currentClip->w) / 2, 
            (SCREEN_HEIGHT - currentClip->h) / 2, 
            currentClip
        );

        // Update screen
        SDL_RenderPresent(gRenderer);

        ++frame;

        // Cycle animation
        if (frame / 4 >= WALKING_ANIMATION_FRAMES)
            frame = 0;
    }

    close();

    return SUCCESS;
}


bool loadMedia()
{
    if (!gSpriteSheetTexture.loadFromFile("foo.png")) {
        printf("Failed to load sprite sheet texture.\n");
        return false;
    }

    //Set sprite clips
    gSpriteClips[ 0 ].x =   0;
    gSpriteClips[ 0 ].y =   0;
    gSpriteClips[ 0 ].w =  64;
    gSpriteClips[ 0 ].h = 205;

    gSpriteClips[ 1 ].x =  64;
    gSpriteClips[ 1 ].y =   0;
    gSpriteClips[ 1 ].w =  64;
    gSpriteClips[ 1 ].h = 205;
    
    gSpriteClips[ 2 ].x = 128;
    gSpriteClips[ 2 ].y =   0;
    gSpriteClips[ 2 ].w =  64;
    gSpriteClips[ 2 ].h = 205;

    gSpriteClips[ 3 ].x = 196;
    gSpriteClips[ 3 ].y =   0;
    gSpriteClips[ 3 ].w =  64;
    gSpriteClips[ 3 ].h = 205;

    return true;
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
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
