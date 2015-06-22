#include <windows.h>


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

            while (GlobalRunning)
            {
                MSG Message;
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                else
                {
                    GlobalRunning = false;
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