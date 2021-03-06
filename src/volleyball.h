#include <stdint.h>
#include <stdio.h>

#include "volleyball_platform.h"
#include "volleyball_math.h"


struct game_offscreen_buffer
{
    void *Memory;
    int Width;
    int Height;
    int BytesPerPixel;
    int MaxWidth;   // We'll only allocate this much
    int MaxHeight;
};


struct bmp_file
{
    int Width;
    int Height;
    void *Bitmap;

    // Zero if compression is not 3
    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
    u32 AlphaMask;
};


#pragma pack(push, 1)

struct bmp_file_header {
    u16 bfType;
    u32 bfSize;
    u16 bfReserved1;
    u16 bfReserved2;
    u32 bfOffBits;
};


struct bmp_info_header {
    u32 biSize;
    i32 biWidth;
    i32 biHeight;
    u16 biPlanes;
    u16 biBitCount;
    u32 biCompression;
    u32 biSizeImage;
    i32 biXPelsPerMeter;
    i32 biYPelsPerMeter;
    u32 biClrUsed;
    u32 biClrImportant;
    u32 biRedMask;
    u32 biGreenMask;
    u32 biBlueMask;
    u32 biAlphaMask;
};

#pragma pack(pop)


struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};


struct player_input
{
    union
    {
        game_button_state Buttons[4];

        struct
        {
            game_button_state Up;
            game_button_state Down;
            game_button_state Left;
            game_button_state Right;
        };
    };
};


union game_input
{
    player_input Players[2];
    struct
    {
        player_input Player1;
        player_input Player2;
    };
    r32 dtForFrame;
};


struct entity
{
    v2 Position;
    v2 Velocity;
    bmp_file Image;

    // tmp
    v2 Center;
    int Radius;
    r32 Mass;
    bool32 NotJumped;
};


struct player
{
    entity Entity;

};


struct file_read_result
{
    void *Memory;
    u64 MemorySize;
};


// Platform functions

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) file_read_result name(char *Filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) void name(char *Filename, int FileSize, void *Memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);


struct game_memory
{
    int MemorySize;
    bool32 IsInitialized;
    void *Start;
    void *Free;

    // Debug functions
    debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
};


// Game functions

#define GAME_UPDATE_AND_RENDER(name) void name(game_input *NewInput, game_offscreen_buffer *Buffer, game_memory *Memory)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
    // nothing
}

