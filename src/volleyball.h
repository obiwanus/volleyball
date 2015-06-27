#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float r32;
typedef double r64;
typedef i32 bool32;

#define internal static
#define global static
#define local_persist static

#if BUILD_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif


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


struct debug_image
{
    int X;
    int Y;
    int MinWidth;
    int MaxWidth;
    int DirX;
    int DirY;
    int DirWidth;
    u32 Color;
    bmp_file BMPFile;  
};


struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};


struct game_input
{
    game_button_state Up;
    game_button_state Down;
    game_button_state Left;
    game_button_state Right;
};


struct file_read_result
{
    void *Memory;
    u64 MemorySize;
};


// Platform functions
file_read_result DEBUGPlatformReadEntireFile(char *Filename);
void *DEBUGPlatformWriteEntireFile(char *Filename, int FileSize, void *Memory);

