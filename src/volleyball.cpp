
global game_memory GameMemory;
global game_offscreen_buffer GameBackBuffer;


internal void *
GameMemoryAlloc(int SizeInBytes)
{
    void *Result = GameMemory.Free;

    GameMemory.Free = (void *)((u8 *)GameMemory.Free + SizeInBytes);
    i64 CurrentSize = ((u8 *)GameMemory.Free - (u8 *) GameMemory.Start);
    if (CurrentSize >= GameMemory.MemorySize)
    {
        return 0;  // not enough memory
    }

    return Result;
}


internal void
GameUpdateAndRender()
{
    u32 X = 0, Y = 0;
    u32 Width = 400, Height = 300;

    u32 Pitch = GameBackBuffer.Width * GameBackBuffer.BytesPerPixel;
    u8 *Row = (u8 *) GameBackBuffer.Memory + Pitch * Y + X * GameBackBuffer.BytesPerPixel;
    
    for (u32 pY = Y; pY < Y + Height; pY++)
    {
        u32 *Pixel = (u32 *) Row;
        for (u32 pX = X; pX < X + Width; pX++)
        {
            *Pixel++ = 0x00FF00FF;
            // u8 Red = 0xFF;
            // u8 Green = 0xFF;
            // u8 Blue = 0;
            // *Pixel++ = Red << 16 | Green << 8 | Blue;
        }
        Row += Pitch;
    }

    // // TMP: square
    // i32 X = 100, Y = 100;
    // i32 XDirection = 3, YDirection = 3;
    // i32 Width = 50, Height = 50;
    // i32 MaxX = GlobalWin32BackBuffer.Width - Width;
    // i32 MaxY = GlobalWin32BackBuffer.Height - Height;

    // // Move and draw square
    // {
    //     Win32DrawSquare(X, Y, Width, Height, 0x00000000);  // erase

    //     X += XDirection;
    //     Y += YDirection;

    //     if (X <= 0) { X = 0; XDirection = -XDirection; }
    //     if (Y <= 0) { Y = 0; YDirection = -YDirection; }
    //     if (X > MaxX) { X = MaxX; XDirection = -XDirection; }
    //     if (Y > MaxY) { Y = MaxY; YDirection = -YDirection; }

    //     Win32DrawSquare(X, Y, Width, Height, 0x00FFFF00);
    // }
}