#include "volleyball.cpp"

#include <windows.h>
#include <intrin.h>

#include "win32_volleyball.h"

global bool GlobalRunning;

global BITMAPINFO GlobalBitmapInfo;
global LARGE_INTEGER GlobalPerformanceFrequency;


file_read_result
DEBUGPlatformReadEntireFile(char *Filename)
{
    file_read_result Result = {};

    HANDLE FileHandle = CreateFile(
        Filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(FileHandle, &FileSize))
        {
            Result.MemorySize = FileSize.QuadPart;
            Result.Memory = VirtualAlloc(0, Result.MemorySize, MEM_COMMIT, PAGE_READWRITE);
            DWORD BytesRead = 0;

            ReadFile(
                FileHandle,
                Result.Memory,
                (u32)Result.MemorySize,
                &BytesRead,
                0);

            CloseHandle(FileHandle);
            
            return Result;
        }
        else
        {
            OutputDebugStringA("Cannot get file size\n");
            // GetLastError() should help
        }    
    }
    else
    {
        OutputDebugStringA("Cannot read from file\n");
        // GetLastError() should help
    }

    return Result;
}


internal void
Win32UpdateWindow(HDC hdc)
{
    StretchDIBits(
        hdc,
        0, 0, GameBackBuffer.Width, GameBackBuffer.Height,  // dest
        0, 0, GameBackBuffer.Width, GameBackBuffer.Height,  // src
        GameBackBuffer.Memory,
        &GlobalBitmapInfo,
        DIB_RGB_COLORS, SRCCOPY);
}


internal void
Win32ResizeClientWindow(HWND Window)
{
    if (!GameMemory.IsInitialized)
        return;  // no buffer yet

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    int Width = ClientRect.right - ClientRect.left;
    int Height = ClientRect.bottom - ClientRect.top;

    if (Width > GameBackBuffer.MaxWidth)
    {
        Width = GameBackBuffer.MaxWidth;
    }
    if (Height > GameBackBuffer.MaxHeight)
    {
        Height = GameBackBuffer.MaxHeight;
    }

    GameBackBuffer.Width = Width;
    GameBackBuffer.Height = Height;

    GlobalBitmapInfo.bmiHeader.biWidth = Width;
    GlobalBitmapInfo.bmiHeader.biHeight = -Height;
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
            Win32ResizeClientWindow(hwnd);
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

            OutputDebugStringA("WM_PAINT\n");
            
            EndPaint(hwnd, &Paint);
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            Assert(!"Keyboard input came in through a non-dispatch message!");
        } break;

        default:
        {
            Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        } break;
    }

    return Result;
}


internal void
Win32ProcessPendingMessages(game_input *NewInput)
{
    MSG Message;
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        // Get keyboard messages
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            } break;

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                u32 VKCode = (u32)Message.wParam;
                bool32 WasDown = ((Message.lParam & (1 << 30)) != 0);
                bool32 IsDown = ((Message.lParam & (1 << 31)) == 0);

                // Get input
                {
                    if(VKCode == 'W' || VKCode == VK_UP)
                    {
                        NewInput->Up.HalfTransitionCount++;
                        NewInput->Up.EndedDown = IsDown;
                    }
                    else if(VKCode == 'S' || VKCode == VK_DOWN)
                    {
                        NewInput->Down.HalfTransitionCount++;
                        NewInput->Down.EndedDown = IsDown;
                    }
                    else if(VKCode == 'A' || VKCode == VK_LEFT)
                    {
                        NewInput->Left.HalfTransitionCount++;
                        NewInput->Left.EndedDown = IsDown;
                    }
                    else if(VKCode == 'D' || VKCode == VK_RIGHT)
                    {
                        NewInput->Right.HalfTransitionCount++;
                        NewInput->Right.EndedDown = IsDown;
                    }
                }

                bool32 AltKeyWasDown = (Message.lParam & (1 << 29));
                if((VKCode == VK_F4) && AltKeyWasDown)
                {
                    GlobalRunning = false;
                }
            } break;

            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            } break;
        }
    }
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
        int WindowWidth = 1024;
        int WindowHeight = 768;

        HWND Window = CreateWindow(
            WindowClass.lpszClassName,
            0,
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            WindowWidth,
            WindowHeight,
            0,
            0,
            hInstance,
            0);

        // We're not going to release it as we use CS_OWNDC
        HDC hdc = GetDC(Window);

        if (Window)
        {
            GlobalRunning = true;

            LARGE_INTEGER LastTimestamp = Win32GetWallClock();

            // Init game memory
            {
                GameMemory.MemorySize = 1024 * 1024 * 1024;  // 1 Gigabyte
                GameMemory.Start = VirtualAlloc(0, GameMemory.MemorySize, MEM_COMMIT, PAGE_READWRITE);
                GameMemory.Free = GameMemory.Start;
                GameMemory.IsInitialized = true;
            }

            // Init backbuffer
            {
                GameBackBuffer.MaxWidth = 2000;
                GameBackBuffer.MaxHeight = 1500;
                GameBackBuffer.BytesPerPixel = 4;

                int BufferSize = GameBackBuffer.MaxWidth * GameBackBuffer.MaxHeight 
                                  * GameBackBuffer.BytesPerPixel;
                GameBackBuffer.Memory = GameMemoryAlloc(BufferSize);                                  
                
                GlobalBitmapInfo.bmiHeader.biSize = sizeof(GlobalBitmapInfo.bmiHeader);
                GlobalBitmapInfo.bmiHeader.biPlanes = 1;
                GlobalBitmapInfo.bmiHeader.biBitCount = 32;
                GlobalBitmapInfo.bmiHeader.biCompression = BI_RGB;

                // Set up proper values of buffers based on actual client size
                Win32ResizeClientWindow(Window);
            }

            // Get space for inputs
            game_input *OldInput = (game_input *) GameMemoryAlloc(sizeof(game_input));
            game_input *NewInput = (game_input *) GameMemoryAlloc(sizeof(game_input));;

            // Main loop
            while (GlobalRunning)
            {
                // Collect input
                Win32ProcessPendingMessages(NewInput);

                GameUpdateAndRender(NewInput);

                // Swap inputs
                game_input *TmpInput = OldInput;
                OldInput = NewInput;
                NewInput = TmpInput;
                *NewInput = {};  // zero everything


                Win32UpdateWindow(hdc);

                // Enforce FPS
                // TODO: for some reason Time to sleep drops every now and again,
                // disabling gradient solves or masks this, though I don't see
                // any reason why this might happen
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

                    // if (TimeToSleep)
                    // {
                    //     char String[300];
                    //     sprintf_s(String, "Time to sleep: %d, Ms elapsed: %.2f, < 10 = %d\n", TimeToSleep, MillisecondsElapsed, TimeToSleep < 10);
                    //     OutputDebugStringA(String);
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