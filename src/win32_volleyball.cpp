#include <windows.h>
#include <stdint.h>
#include <intrin.h>
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

#define internal static
#define global static
#define local_persist static


struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int BytesPerPixel;
};


global bool GlobalRunning;

global win32_offscreen_buffer GlobalOffscreenBuffer;
global LARGE_INTEGER GlobalPerformanceFrequency;


internal void
Win32DrawGradient(int XOffset, int YOffset)
{
    u8 *Row = (u8 *) GlobalOffscreenBuffer.Memory;
    int Pitch = GlobalOffscreenBuffer.Width * GlobalOffscreenBuffer.BytesPerPixel;
    
    for (int Y = 0; Y < GlobalOffscreenBuffer.Height; Y++)
    {
        u32 *Pixel = (u32 *) Row;
        for (int X = 0; X < GlobalOffscreenBuffer.Width; X++)
        {
            u8 Red = (u8) (X + XOffset);
            u8 Green = (u8) (Y + YOffset);
            u8 Blue = 0;
            *Pixel++ = Red << 16 | Green << 8 | Blue;
        }
        Row += Pitch;
    }
}


internal void
Win32UpdateWindow(HDC hdc)
{
    StretchDIBits(
        hdc,
        0, 0, GlobalOffscreenBuffer.Width, GlobalOffscreenBuffer.Height,  // dest
        0, 0, GlobalOffscreenBuffer.Width, GlobalOffscreenBuffer.Height,  // src
        GlobalOffscreenBuffer.Memory,
        &GlobalOffscreenBuffer.Info,
        DIB_RGB_COLORS, SRCCOPY);
}


internal void
Win32ResizeDIBSection(int Width, int Height)
{
    if (GlobalOffscreenBuffer.Memory)
    {
        VirtualFree(GlobalOffscreenBuffer.Memory, 0, MEM_RELEASE);
    }

    GlobalOffscreenBuffer.Width = Width;
    GlobalOffscreenBuffer.Height = Height;
    GlobalOffscreenBuffer.BytesPerPixel = 4;

    GlobalOffscreenBuffer.Info.bmiHeader.biSize = sizeof(GlobalOffscreenBuffer.Info.bmiHeader);
    GlobalOffscreenBuffer.Info.bmiHeader.biWidth = Width;
    GlobalOffscreenBuffer.Info.bmiHeader.biHeight = -Height;
    GlobalOffscreenBuffer.Info.bmiHeader.biPlanes = 1;
    GlobalOffscreenBuffer.Info.bmiHeader.biBitCount = 32;
    GlobalOffscreenBuffer.Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (Width * Height) * GlobalOffscreenBuffer.BytesPerPixel;
    GlobalOffscreenBuffer.Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}


inline LARGE_INTEGER 
Win32GetWallClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);

    return Result;
}


inline r32
Win32GetMillisecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    r32 Result = 1000.0f * (r32) (End.QuadPart - Start.QuadPart) / 
                 (r32) GlobalPerformanceFrequency.QuadPart;

    return Result;
}


LRESULT CALLBACK 
Win32WindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    LRESULT Result = 0;

    switch(uMsg)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(hwnd, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
        } break;

        case WM_CLOSE:
        {
            GlobalRunning = false;
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint = {};
            HDC hdc = BeginPaint(hwnd, &Paint);
            Win32UpdateWindow(
                hdc  // we update the whole window, always
                // Paint.rcPaint.left,
                // Paint.rcPaint.top,
                // Paint.rcPaint.right - Paint.rcPaint.left,
                // Paint.rcPaint.bottom - Paint.rcPaint.top
            );
            
            EndPaint(hwnd, &Paint);
        } break;

        default:
        {
            Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        } break;
    }

    return Result;
}


int CALLBACK 
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    WNDCLASS WindowClass = {};
    WindowClass.style = CS_OWNDC|CS_VREDRAW|CS_HREDRAW;
    WindowClass.lpfnWndProc = Win32WindowProc;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "VolleyballWindowClass";

    // TODO: query monitor refresh rate
    int TargetFPS = 60;
    r32 TargetMSPF = 1000.0f / (r32)TargetFPS;  // Target ms per frame

    // Set target sleep resolution
    {
        #define TARGET_SLEEP_RESOLUTION 1   // 1-millisecond target resolution

        TIMECAPS tc;
        UINT     wTimerRes;

        if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
        {
            OutputDebugStringA("Cannot set the sleep resolution\n");
            exit(1);
        }

        wTimerRes = min(max(tc.wPeriodMin, TARGET_SLEEP_RESOLUTION), tc.wPeriodMax);
        timeBeginPeriod(wTimerRes);
    }
    
    QueryPerformanceFrequency(&GlobalPerformanceFrequency); 

    if (RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindow(
            WindowClass.lpszClassName,
            0,
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            1024,
            768,
            0,
            0,
            hInstance,
            0);

        if (Window)
        {
            GlobalRunning = true;
            int XOffset = 0;
            int YOffset = 0;

            LARGE_INTEGER LastTimestamp = Win32GetWallClock();

            // Main loop
            while (GlobalRunning)
            {
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }

                Win32DrawGradient(XOffset++, YOffset++);
                HDC hdc = GetDC(Window);
                Win32UpdateWindow(hdc);
                ReleaseDC(Window, hdc);

                // Enforce FPS
                {
                    r32 MillisecondsElapsed = Win32GetMillisecondsElapsed(LastTimestamp, Win32GetWallClock());
                    u32 TimeToSleep = 0;

                    if (MillisecondsElapsed < TargetMSPF)
                    {
                        TimeToSleep = (u32)(TargetMSPF - MillisecondsElapsed);
                        Sleep(TimeToSleep);

                        while (MillisecondsElapsed < TargetMSPF)
                        {
                            MillisecondsElapsed = Win32GetMillisecondsElapsed(LastTimestamp, Win32GetWallClock());
                        }
                    }
                    else
                    {
                        OutputDebugStringA("Frame missed\n");
                    }

                    LastTimestamp = Win32GetWallClock();
                    char String[300];
                    sprintf_s(String, "Time to sleep: %d, Target MSPF: %.2f, Milliseconds elapsed: %.2f\n", TimeToSleep, TargetMSPF, MillisecondsElapsed);
                    OutputDebugStringA(String);

                    // if (ElapsedMicroseconds.QuadPart < TargetMSPF * 1000)
                    // {
                    //     u32 TimeToSleep = (u32)(TargetMSPF - ElapsedMicroseconds.QuadPart / 1000) ;
                    //     // char String[300];
                    //     // sprintf_s(String, "Time to sleep: %d, Target MSPF: %d, Elapsed microseconds: %ld", TimeToSleep, TargetMSPF, ElapsedMicroseconds.QuadPart);
                    //     // OutputDebugStringA(String);
                    //     Sleep(TimeToSleep);
                    // }
                    // else
                    // {
                    //     OutputDebugStringA("Frame missed\n");
                    // }
                }
            }
        }
    }
    else
    {
        // TODO: logging
        OutputDebugStringA("Couldn't register window class");
    }

    return 0;
}