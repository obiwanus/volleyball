#include <stdint.h>
#include <stdio.h>


struct game_memory
{
    int MemorySize;
    bool32 IsInitialized;
    void *Start;
    void *Free;
};


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
};


struct player
{
    v2 Position;
    v2 Velocity;
    bmp_file BMPFile;
};


struct file_read_result
{
    void *Memory;
    u64 MemorySize;
};


// Platform functions
file_read_result DEBUGPlatformReadEntireFile(char *Filename);
void *DEBUGPlatformWriteEntireFile(char *Filename, int FileSize, void *Memory);

