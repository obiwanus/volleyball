#include <windows.h>


#define global static
#define local_persist static


global bool GlobalRunning;


LRESULT CALLBACK 
VolleyballWindowProc(
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
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_CLOSE:
        {
            GlobalRunning = false;
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint = {};
            HDC hdc = BeginPaint(hwnd, &Paint);
            PatBlt(hdc,
                   Paint.rcPaint.left,
                   Paint.rcPaint.top,
                   Paint.rcPaint.right - Paint.rcPaint.left,
                   Paint.rcPaint.bottom - Paint.rcPaint.top,
                   BLACKNESS);
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
    WindowClass.lpfnWndProc = VolleyballWindowProc;
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