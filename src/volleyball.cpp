#include "volleyball_math.h"
#include "volleyball.h"


global game_memory GameMemory;
global game_offscreen_buffer GameBackBuffer;

global entity *Players;
global entity *Ball;


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
DEBUGDrawRectangle(v2 Position, int Width, int Height, u32 Color)
{
    int X = (int)Position.x;
    int Y = (int)Position.y;

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
DEBUGDrawCircle(v2 Center, int Radius, u32 Color)
{
    int X = (int)Center.x - Radius;
    int Y = (int)Center.y - Radius;
    int Height = Radius * 2;
    int Width = Radius * 2;
    int CenterX = (int)Center.x;
    int CenterY = (int)Center.y;
    int RadiusSq = Radius * Radius;

    int Pitch = GameBackBuffer.Width * GameBackBuffer.BytesPerPixel;
    u8 *Row = (u8 *)GameBackBuffer.Memory + Pitch * Y + X * GameBackBuffer.BytesPerPixel;

    for (int pY = Y; pY < Y + Height; pY++)
    {
        int *Pixel = (int *) Row;
        for (int pX = X; pX < X + Width; pX++)
        {
            int nX = CenterX - pX;
            int nY = CenterY - pY;
            if ((nX * nX + nY * nY) <= RadiusSq)
            {
                *Pixel = Color;
            }
            Pixel++;
        }
        Row += Pitch;
    }
}


internal void
DEBUGDrawImage(v2 Position, bmp_file Image)
{
    int Width = Image.Width;
    int Height = Image.Width;
    int X = (int)Position.x;
    int Y = (int)Position.y;

    int Pitch = GameBackBuffer.Width * GameBackBuffer.BytesPerPixel;
    u8 *Row = (u8 *)GameBackBuffer.Memory + Pitch * Y + X * GameBackBuffer.BytesPerPixel;
    u32 *SrcRow = (u32 *)Image.Bitmap + Height * Width - Width;  // bottom row first

    for (int pY = Y; pY < Y + Height; pY++)
    {
        int *Pixel = (int *) Row;
        int *SrcPixel = (int *) SrcRow;

        for (int pX = X; pX < X + Width; pX++)
        {
            // TODO: bmp may not be masked
            u8 Red = UnmaskColor(*SrcPixel, Image.RedMask);
            u8 Green = UnmaskColor(*SrcPixel, Image.GreenMask);
            u8 Blue = UnmaskColor(*SrcPixel, Image.BlueMask);
            u8 Alpha = UnmaskColor(*SrcPixel, Image.AlphaMask);

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
InitEntity(entity *Entity, char *ImgPath, v2 Position, v2 Velocity,
           v2 Center, int Radius, r32 Mass)
{
    bmp_file BMPFile = DEBUGReadBMPFile(ImgPath);
    Entity->Image = BMPFile;
    Entity->Position = Position;
    Entity->Velocity = Velocity;

    Entity->Center = Center;
    Entity->Radius = Radius;
    Entity->Mass = Mass;
}


internal void
CollideWithWalls(entity *Entity)
{
    r32 MinX = 0.0f;
    r32 MinY = 0.0f;
    r32 MaxX = (r32)(GameBackBuffer.Width - Entity->Image.Width);
    r32 MaxY = (r32)(GameBackBuffer.Height - Entity->Image.Height);

    if (Entity->Position.x < MinX)
    {
        Entity->Position.x = MinX;
        Entity->Velocity.x = -Entity->Velocity.x;
    }
    if (Entity->Position.y < MinY)
    {
        Entity->Position.y = MinY;
        Entity->Velocity.y = -Entity->Velocity.y;
    }
    if (Entity->Position.x > MaxX)
    {
        Entity->Position.x = MaxX;
        Entity->Velocity.x = -Entity->Velocity.x;
    }
    if (Entity->Position.y > MaxY)
    {
        Entity->Position.y = MaxY;
        Entity->Velocity.y = -Entity->Velocity.y;
    }
}


internal void
UpdatePlayer(entity *Player, player_input *Input, r32 dtForFrame)
{
    v2 Direction = {};

    if (Input->Up.EndedDown)
        Direction.y -= 1.0f;
    if (Input->Down.EndedDown)
        Direction.y += 1.0f;
    if (Input->Right.EndedDown)
        Direction.x += 1.0f;
    if (Input->Left.EndedDown)
        Direction.x -= 1.0f;

    if (Direction.x != 0 && Direction.y != 0)
    {
        Direction *= 0.70710678118f;
    }

    Direction *= 0.25f;  // speed, px/ms
    Direction -= 0.2f * Player->Velocity;  // friction

    Player->Velocity += Direction;

    Player->Position += Player->Velocity * dtForFrame;

    CollideWithWalls(Player);

    // char Buffer[256];
    // sprintf_s(Buffer, "%.2f, %.2f\n", Player->Position.x, Player->Position.y);
    // OutputDebugStringA(Buffer);
}


internal void
GameUpdateAndRender(game_input *NewInput)
{
    if (!Players)
    {
        Players = (entity *)GameMemoryAlloc(sizeof(entity) * 2);  // 2 players
        Ball = (entity *)GameMemoryAlloc(sizeof(entity) * 2);  // 1 ball

        InitEntity(
            &Players[0],
            "./img/player.bmp",
            {100, 400},
            {0, 0},
            {64, 69},
            60,
            4);

        InitEntity(
            &Players[1],
            "./img/player.bmp",
            {600, 400},
            {0, 0},
            {64, 69},
            60,
            4);

        InitEntity(
            Ball,
            "./img/ball.bmp",
            {300, 100},
            {0, 0},
            // {-0.5f, -0.5f},
            {36, 35},
            32,
            10);
    }

    // Move and draw image
    DEBUGDrawRectangle({0, 0}, GameBackBuffer.Width, GameBackBuffer.Height, 0x00002222);  // OMG

    // Update players
    {
        UpdatePlayer(&Players[0], &NewInput->Players[0], NewInput->dtForFrame);
        UpdatePlayer(&Players[1], &NewInput->Players[1], NewInput->dtForFrame);
    }

    // Update ball
    {
        Ball->Position += Ball->Velocity * NewInput->dtForFrame;
        CollideWithWalls(Ball);
    }

    // Ball-players collisions
    for (int i = 0; i < COUNT_OF(NewInput->Players); i++)
    {
        entity *Player = &Players[i];
        v2 pCenter = Player->Position + Player->Center;
        v2 bCenter = Ball->Position + Ball->Center;
        if (DistanceBetween(pCenter, bCenter) < (Player->Radius + Ball->Radius))
        {
            // Collision!
            v2 CollisionNormal = (bCenter - pCenter) * (1.0f / DistanceBetween(bCenter, pCenter));

            // The components of the velocities collinear with the normal
            v2 BallComponent = CollisionNormal * DotProduct(Ball->Velocity, CollisionNormal);
            v2 PlayerComponent = CollisionNormal * DotProduct(Player->Velocity, CollisionNormal);
            v2 ResultingV = BallComponent - PlayerComponent;

            v2 ResultingBallV = -ResultingV * ((Ball->Mass + Player->Mass) / (2.0f * Ball->Mass));
            v2 ResultingPlayerV = ResultingV * ((Ball->Mass + Player->Mass) / (2.0f * Player->Mass));

            Ball->Velocity += ResultingBallV - BallComponent;
            Player->Velocity += ResultingPlayerV - PlayerComponent;
        }
    }

    // Draw
    {
        DEBUGDrawImage(Players[0].Position, Players[0].Image);
        DEBUGDrawImage(Players[1].Position, Players[1].Image);
        DEBUGDrawImage(Ball->Position, Ball->Image);

        // DEBUGDrawCircle(Players[0].Position + Players[0].Center, Players[0].Radius, 0x00FFFFFF);
    }
}


