#include "volleyball.h"

global game_memory GameMemory;
global game_offscreen_buffer GameBackBuffer;

global debug_image *DebugImage;


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
DEBUGDrawImage(debug_image *Image, u32 Color)
{
    int Width = Image->BMPFile.Width;
    int Height = Image->BMPFile.Width;
    int X = Image->X; 
    int Y = Image->Y;

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
DEBUGEraseImage(debug_image *Image)
{

}


internal bmp_file
DEBUGReadBMPFile(char *Filename)
{
    bmp_file Result = {};
    file_read_result FileReadResult = DEBUGPlatformReadEntireFile(Filename);

    bmp_file_header *BMPFileHeader = (bmp_file_header *)FileReadResult.Memory;
    bmp_info_header *BMPInfoHeader = (bmp_info_header *)((u8 *)FileReadResult.Memory 
                                                          + sizeof(bmp_file_header));

    Result.Bitmap = (void *) ((u8 *)FileReadResult.Memory + 
                              BMPFileHeader->bfOffBits);
    Result.Width = BMPInfoHeader->biWidth;
    Result.Height = BMPInfoHeader->biHeight;

    if (BMPInfoHeader->biCompression == 3)  // BI_BITFIELDS
    {
        Result.RedMask = BMPInfoHeader->biRedMask;
        Result.GreenMask = BMPInfoHeader->biGreenMask;
        Result.BlueMask = BMPInfoHeader->biBlueMask;
        Result.AlphaMask = BMPInfoHeader->biAlphaMask;
    }

    return Result;
}


internal void
GameUpdateAndRender(game_input *NewInput)
{
    if (!DebugImage)
    {
        bmp_file BMPFile = DEBUGReadBMPFile("./img/ball.bmp");
        DebugImage->BMPFile = BMPFile;
        DebugImage->X = 100; 
        DebugImage->Y = 100;
        DebugImage->DirX = 3;
        DebugImage->DirY = 3;
        DebugImage->MinWidth = 10;
        DebugImage->MaxWidth = 150;
        DebugImage->DirWidth = -1;
        DebugImage->Color = 0x0000FFFF;
    }

    // Move and draw image
    {
        DEBUGEraseImage(DebugImage);

        if (NewInput->Up.EndedDown)
            DebugImage->DirY -= NewInput->Up.HalfTransitionCount;
        if (NewInput->Down.EndedDown)
            DebugImage->DirY += NewInput->Down.HalfTransitionCount;
        if (NewInput->Right.EndedDown)
            DebugImage->DirX += NewInput->Right.HalfTransitionCount;
        if (NewInput->Left.EndedDown)
            DebugImage->DirX -= NewInput->Left.HalfTransitionCount;

        if (DebugImage->DirX > DebugImage->MaxWidth)
            DebugImage->DirX = DebugImage->MaxWidth;
        if (DebugImage->DirX <= -DebugImage->MaxWidth)
            DebugImage->DirX = -DebugImage->MaxWidth;

        if (DebugImage->DirY > DebugImage->MaxWidth)
            DebugImage->DirY = DebugImage->MaxWidth;
        if (DebugImage->DirY <= -DebugImage->MaxWidth)
            DebugImage->DirY = -DebugImage->MaxWidth;

        DebugImage->X += DebugImage->DirX;
        DebugImage->Y += DebugImage->DirY;

        int MinX = 0;
        int MinY = 0;
        int MaxX = GameBackBuffer.Width;
        int MaxY = GameBackBuffer.Height;

        if (DebugImage->X < MinX) { DebugImage->X = MinX; DebugImage->DirX = -DebugImage->DirX; }
        if (DebugImage->Y < MinY) { DebugImage->Y = MinY; DebugImage->DirY = -DebugImage->DirY; }
        if (DebugImage->X > MaxX) { DebugImage->X = MaxX; DebugImage->DirX = -DebugImage->DirX; }
        if (DebugImage->Y > MaxY) { DebugImage->Y = MaxY; DebugImage->DirY = -DebugImage->DirY; }

        u8 Red = (u8) DebugImage->X;
        u8 Green = (u8) DebugImage->Y;
        u8 Blue = 0;
        DebugImage->Color = Red << 16 | Green << 8 | Blue;

        DEBUGDrawImage(DebugImage, DebugImage->Color);
    }
}