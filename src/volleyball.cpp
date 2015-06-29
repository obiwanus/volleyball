#include "volleyball_math.h"
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


inline u8
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
    int X = (int)Image->Position.x;
    int Y = (int)Image->Position.y;

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
        DebugImage->Position = {(r32)GameBackBuffer.Width / 2.0f,
                                (r32)GameBackBuffer.Height / 2.0f};
        DebugImage->Velocity = {};
    }

    // Move and draw image
    {
        DEBUGDrawRectangle(0, 0, GameBackBuffer.Width, GameBackBuffer.Height, 0x00002222);  // OMG

        v2 PlayerDirection = {};

        if (NewInput->Up.EndedDown)
            PlayerDirection.y -= 1.0f;
        if (NewInput->Down.EndedDown)
            PlayerDirection.y += 1.0f;
        if (NewInput->Right.EndedDown)
            PlayerDirection.x += 1.0f;
        if (NewInput->Left.EndedDown)
            PlayerDirection.x -= 1.0f;

        if (PlayerDirection.x != 0 && PlayerDirection.y != 0)
        {
            PlayerDirection *= 0.70710678118f;
        }

        PlayerDirection *= 2.0f;  // speed
        PlayerDirection -= 0.2f * DebugImage->Velocity;

        DebugImage->Velocity += PlayerDirection;

        // TODO: delta time here
        DebugImage->Position += DebugImage->Velocity;

        // Collisions with walls
        r32 MinX = 0.0f;
        r32 MinY = 0.0f;
        r32 MaxX = (r32)(GameBackBuffer.Width - DebugImage->BMPFile.Width);
        r32 MaxY = (r32)(GameBackBuffer.Height - DebugImage->BMPFile.Height);

        if (DebugImage->Position.x < MinX)
        {
            DebugImage->Position.x = MinX;
            DebugImage->Velocity.x = -DebugImage->Velocity.x;
        }
        if (DebugImage->Position.y < MinY)
        {
            DebugImage->Position.y = MinY;
            DebugImage->Velocity.y = -DebugImage->Velocity.y;
        }
        if (DebugImage->Position.x > MaxX)
        {
            DebugImage->Position.x = MaxX;
            DebugImage->Velocity.x = -DebugImage->Velocity.x;
        }
        if (DebugImage->Position.y > MaxY)
        {
            DebugImage->Position.y = MaxY;
            DebugImage->Velocity.y = -DebugImage->Velocity.y;
        }

        DEBUGDrawImage(DebugImage);
    }
}