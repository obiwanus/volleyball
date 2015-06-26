
global game_memory GameMemory;
global game_offscreen_buffer GameBackBuffer;

global debug_square *DebugSquare;


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
    int X = Square->X; 
    int Y = Square->Y;
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
GameUpdateAndRender()
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
        DebugSquare->DirWidth = -2;
        DebugSquare->Color = 0x0000FFFF;

        GameMemory.IsInitialized = true;
    }

    // Move and draw square
    {
        DEBUGDrawRectangle(DebugSquare, 0x00000000);  // erase

        DebugSquare->X += DebugSquare->DirX;
        DebugSquare->Y += DebugSquare->DirY;
        DebugSquare->Width += DebugSquare->DirWidth;

        int MaxX = GameBackBuffer.Width - DebugSquare->Width;
        int MaxY = GameBackBuffer.Height - DebugSquare->Width;

        if (DebugSquare->X <= 0) { DebugSquare->X = 0; DebugSquare->DirX = -DebugSquare->DirX; }
        if (DebugSquare->Y <= 0) { DebugSquare->Y = 0; DebugSquare->DirY = -DebugSquare->DirY; }
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