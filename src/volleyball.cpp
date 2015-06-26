#include "volleyball.h"

global game_memory GameMemory;
global game_offscreen_buffer GameBackBuffer;

global debug_square *DebugSquare;


inline int
TruncateReal32(r32 Value)
{
    int Result = (int) Value;
    return Result;
}


inline int
RoundReal32(r32 Value)
{
    // TODO: think about overflow
    return TruncateReal32(Value + 0.5f);
}


internal void *
GameMemoryAlloc(int SizeInBytes)
{
    void *Result = GameMemory.Free;

    GameMemory.Free = (void *)((u8 *)GameMemory.Free + SizeInBytes);
    i64 CurrentSize = ((u8 *)GameMemory.Free - (u8 *) GameMemory.Start);
    Assert(CurrentSize < GameMemory.MemorySize);

    return Result;
}


internal void
DEBUGDrawRectangle(debug_square *Square, u32 Color)
{
    int HalfSize = RoundReal32((r32)Square->Width / 2.0f);
    int X = Square->X - HalfSize; 
    int Y = Square->Y - HalfSize;
    int Width = Square->Width;
    int Height = Square->Width;

    int Pitch = GameBackBuffer.Width * GameBackBuffer.BytesPerPixel;
    u8 *Row = (u8 *) GameBackBuffer.Memory + Pitch * Y + X * GameBackBuffer.BytesPerPixel;
    
    for (int pY = Y; pY < Y + Height; pY++)
    {
        int *Pixel = (int *) Row;
        for (int pX = X; pX < X + Width; pX++)
        {
            *Pixel++ = Color;
            // u8 Red = 0xFF;
            // u8 Green = 0xFF;
            // u8 Blue = 0;
            // *Pixel++ = Red << 16 | Green << 8 | Blue;
        }
        Row += Pitch;
    }
}


internal void
GameUpdateAndRender(game_input *NewInput)
{
    if (!DebugSquare)
    {
        DebugSquare = (debug_square *)GameMemoryAlloc(sizeof(debug_square));
        DebugSquare->X = 100; 
        DebugSquare->Y = 100;
        DebugSquare->DirX = 3;
        DebugSquare->DirY = 3;
        DebugSquare->Width = 50;
        DebugSquare->MinWidth = 10;
        DebugSquare->MaxWidth = 150;
        DebugSquare->DirWidth = -1;
        DebugSquare->Color = 0x0000FFFF;
    }

    // Move and draw square
    {
        // DEBUGDrawRectangle(DebugSquare, 0x00000000);  // erase

        if (NewInput->Up.EndedDown)
            DebugSquare->DirY -= NewInput->Up.HalfTransitionCount;
        if (NewInput->Down.EndedDown)
            DebugSquare->DirY += NewInput->Down.HalfTransitionCount;
        if (NewInput->Right.EndedDown)
            DebugSquare->DirX += NewInput->Right.HalfTransitionCount;
        if (NewInput->Left.EndedDown)
            DebugSquare->DirX -= NewInput->Left.HalfTransitionCount;

        if (DebugSquare->DirX > DebugSquare->MaxWidth)
            DebugSquare->DirX = DebugSquare->MaxWidth;
        if (DebugSquare->DirX <= -DebugSquare->MaxWidth)
            DebugSquare->DirX = -DebugSquare->MaxWidth;

        if (DebugSquare->DirY > DebugSquare->MaxWidth)
            DebugSquare->DirY = DebugSquare->MaxWidth;
        if (DebugSquare->DirY <= -DebugSquare->MaxWidth)
            DebugSquare->DirY = -DebugSquare->MaxWidth;

        DebugSquare->X += DebugSquare->DirX;
        DebugSquare->Y += DebugSquare->DirY;
        DebugSquare->Width += DebugSquare->DirWidth;

        int HalfSize = RoundReal32((r32)DebugSquare->Width / 2.0f);
        int MinX = HalfSize;
        int MinY = HalfSize;
        int MaxX = GameBackBuffer.Width - HalfSize;
        int MaxY = GameBackBuffer.Height - HalfSize;

        if (DebugSquare->X < MinX) { DebugSquare->X = MinX; DebugSquare->DirX = -DebugSquare->DirX; }
        if (DebugSquare->Y < MinY) { DebugSquare->Y = MinY; DebugSquare->DirY = -DebugSquare->DirY; }
        if (DebugSquare->X > MaxX) { DebugSquare->X = MaxX; DebugSquare->DirX = -DebugSquare->DirX; }
        if (DebugSquare->Y > MaxY) { DebugSquare->Y = MaxY; DebugSquare->DirY = -DebugSquare->DirY; }
        if (DebugSquare->Width > DebugSquare->MaxWidth) 
        { 
            DebugSquare->Width = DebugSquare->MaxWidth; 
            DebugSquare->DirWidth = -DebugSquare->DirWidth; 
        }
        if (DebugSquare->Width <= DebugSquare->MinWidth) 
        { 
            DebugSquare->Width = DebugSquare->MinWidth; 
            DebugSquare->DirWidth = -DebugSquare->DirWidth; 
        }

        u8 Red = (u8) DebugSquare->X;
        u8 Green = (u8) DebugSquare->Y;
        u8 Blue = (u8) DebugSquare->Width;
        DebugSquare->Color = Red << 16 | Green << 8 | Blue;

        DEBUGDrawRectangle(DebugSquare, DebugSquare->Color);
    }
}