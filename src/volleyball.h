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


struct debug_square
{
    int X;
    int Y;
    int Width;
    int MinWidth;
    int MaxWidth;
    int DirX;
    int DirY;
    int DirWidth;
    u32 Color;
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


