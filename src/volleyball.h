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


typedef struct
{
    int MemorySize;
    bool32 IsInitialized;
    void *Start;
    void *Free;
} game_memory;


typedef struct
{
    void *Memory;
    int Width;
    int Height;
    int BytesPerPixel;
    int MaxWidth;   // We'll only allocate this much
    int MaxHeight;
} game_offscreen_buffer;