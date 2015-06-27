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


internal u8
UnmaskColor(u32 Pixel, u32 ColorMask)
{
    int BitOffset = 0;
    switch (ColorMask) 
    {
        case 0x000000FF:
            BitOffset = 0;
            break;
        case 0x0000FF00:
            BitOffset = 8;
            break;
        case 0x00FF0000:
            BitOffset = 16;
            break;
        case 0xFF000000:
            BitOffset = 24;
            break;
    }

    return (u8)((Pixel&ColorMask) >> BitOffset);
}


internal void
DEBUGDrawRectangle(int X, int Y, int Width, int Height, u32 Color)
{
    int Pitch = GameBackBuffer.Width * GameBackBuffer.BytesPerPixel;
    u8 *Row = (u8 *)GameBackBuffer.Memory + Pitch * Y + X * GameBackBuffer.BytesPerPixel;
    
    for (int pY = Y; pY < Y + Height; pY++)
    {
        int *Pixel = (int *) Row;
        for (int pX = X; pX < X + Width; pX++)
        {
            *Pixel++ = Color;
        }
        Row += Pitch;
    }
}


internal void
DEBUGDrawImage(debug_image *Image)
{
    int Width = Image->BMPFile.Width;
    int Height = Image->BMPFile.Width;
    int X = Image->X; 
    int Y = Image->Y;

    int Pitch = GameBackBuffer.Width * GameBackBuffer.BytesPerPixel;
    u8 *Row = (u8 *)GameBackBuffer.Memory + Pitch * Y + X * GameBackBuffer.BytesPerPixel;
    u32 *SrcPixel = (u32 *)Image->BMPFile.Bitmap;
    
    for (int pY = Y; pY < Y + Height; pY++)
    {
        int *Pixel = (int *) Row;
        for (int pX = X; pX < X + Width; pX++)
        {
            // TODO: bmp may not be masked
            u8 Red = UnmaskColor(*SrcPixel, Image->BMPFile.RedMask);
            u8 Green = UnmaskColor(*SrcPixel, Image->BMPFile.GreenMask);
            u8 Blue = UnmaskColor(*SrcPixel, Image->BMPFile.BlueMask);
            u8 Alpha = UnmaskColor(*SrcPixel, Image->BMPFile.AlphaMask);

            u32 ResultingPixel = Alpha << 24 | Red << 16 | Green << 8 | Blue;
            // TODO: blending

            SrcPixel++;

            // *Pixel++ = *SrcPixel++;
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
        DebugImage = (debug_image *)GameMemoryAlloc(sizeof(debug_image));
        DebugImage->BMPFile = BMPFile;
        DebugImage->X = 100; 
        DebugImage->Y = 100;
        DebugImage->DirX = 3;
        DebugImage->DirY = 3;
    }

    // Move and draw image
    {
        DEBUGDrawRectangle(0, 0, GameBackBuffer.Width, GameBackBuffer.Height, 0x00000000);  // OMG

        if (NewInput->Up.EndedDown)
            DebugImage->DirY -= NewInput->Up.HalfTransitionCount;
        if (NewInput->Down.EndedDown)
            DebugImage->DirY += NewInput->Down.HalfTransitionCount;
        if (NewInput->Right.EndedDown)
            DebugImage->DirX += NewInput->Right.HalfTransitionCount;
        if (NewInput->Left.EndedDown)
            DebugImage->DirX -= NewInput->Left.HalfTransitionCount;

        DebugImage->X += DebugImage->DirX;
        DebugImage->Y += DebugImage->DirY;

        int MinX = 0;
        int MinY = 0;
        int MaxX = GameBackBuffer.Width - DebugImage->BMPFile.Width;
        int MaxY = GameBackBuffer.Height - DebugImage->BMPFile.Height;

        if (DebugImage->X < MinX) { DebugImage->X = MinX; DebugImage->DirX = -DebugImage->DirX; }
        if (DebugImage->Y < MinY) { DebugImage->Y = MinY; DebugImage->DirY = -DebugImage->DirY; }
        if (DebugImage->X > MaxX) { DebugImage->X = MaxX; DebugImage->DirX = -DebugImage->DirX; }
        if (DebugImage->Y > MaxY) { DebugImage->Y = MaxY; DebugImage->DirY = -DebugImage->DirY; }

        DEBUGDrawImage(DebugImage);
    }
}