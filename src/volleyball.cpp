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
    u32 *SrcRow = (u32 *)Image->BMPFile.Bitmap + Height * Width - Width;  // bottom row first
    
    for (int pY = Y; pY < Y + Height; pY++)
    {
        int *Pixel = (int *) Row;
        int *SrcPixel = (int *) SrcRow;

        for (int pX = X; pX < X + Width; pX++)
        {
            // TODO: bmp may not be masked
            u8 Red = UnmaskColor(*SrcPixel, Image->BMPFile.RedMask);
            u8 Green = UnmaskColor(*SrcPixel, Image->BMPFile.GreenMask);
            u8 Blue = UnmaskColor(*SrcPixel, Image->BMPFile.BlueMask);
            u8 Alpha = UnmaskColor(*SrcPixel, Image->BMPFile.AlphaMask);

            u32 ResultingColor = Red << 16 | Green << 8 | Blue;

            if (Alpha > 0 && Alpha < 0xFF)
            {
                r32 ExistingRed = (r32)((*Pixel >> 16) & 0xFF);
                r32 ExistingGreen = (r32)((*Pixel >> 8) & 0xFF);
                r32 ExistingBlue = (r32)((*Pixel >> 0) & 0xFF);

                r32 NewRed = (r32)((ResultingColor >> 16) & 0xFF);
                r32 NewGreen = (r32)((ResultingColor >> 8) & 0xFF);
                r32 NewBlue = (r32)((ResultingColor >> 0) & 0xFF);

                // Blending
                r32 t = (r32)Alpha / 255.0f;

                NewRed = NewRed * t + ExistingRed * (1 - t);
                NewGreen = NewGreen * t + ExistingGreen * (1 - t);
                NewBlue = NewBlue * t + ExistingBlue * (1 - t);

                *Pixel = (((u8)NewRed << 16) |
                          ((u8)NewGreen << 8) |
                          ((u8)NewBlue << 0));
            }
            else if (Alpha == 0xFF)
            {
                *Pixel = ResultingColor;
            }
            else
            {
                // do nothing
            }
            
            Pixel++;
            SrcPixel++;
        }
        Row += Pitch;
        SrcRow -= Width;
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
        bmp_file BMPFile = DEBUGReadBMPFile("./img/player_red.bmp");
        DebugImage = (debug_image *)GameMemoryAlloc(sizeof(debug_image));
        DebugImage->BMPFile = BMPFile;
        DebugImage->X = GameBackBuffer.Width / 2; 
        DebugImage->Y = GameBackBuffer.Height / 2;
        DebugImage->DirX = 0;
        DebugImage->DirY = 0;
    }

    // Move and draw image
    {
        DEBUGDrawRectangle(0, 0, GameBackBuffer.Width, GameBackBuffer.Height, 0x00002222);  // OMG

        DebugImage->DirX = 0;
        DebugImage->DirY = 0;

        if (NewInput->Up.EndedDown)
            DebugImage->DirY = -NewInput->Up.HalfTransitionCount;
        if (NewInput->Down.EndedDown)
            DebugImage->DirY = NewInput->Down.HalfTransitionCount;
        if (NewInput->Right.EndedDown)
            DebugImage->DirX = NewInput->Right.HalfTransitionCount;
        if (NewInput->Left.EndedDown)
            DebugImage->DirX = -NewInput->Left.HalfTransitionCount;

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