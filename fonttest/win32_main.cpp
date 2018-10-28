#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#include "default.h"

LRESULT WINAPI win32_window_callback_proc(HWND, UINT, WPARAM, LPARAM);

void query_max_possible_window_size(int *maxw, int *maxh)
{
    *maxw = GetSystemMetrics(SM_CXMAXTRACK);
    *maxh = GetSystemMetrics(SM_CYMAXTRACK);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE __unused1, LPSTR __unused2, int __unused3)
{
    WNDCLASSEX windowclass{};
    windowclass.cbSize        = sizeof(WNDCLASSEX);
    windowclass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowclass.lpfnWndProc   = win32_window_callback_proc;
    windowclass.hInstance     = instance;
    windowclass.lpszClassName = "MyReallyAwesomeWindowClassName";
    windowclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (RegisterClassExA(&windowclass) == 0) Assert(!"RegisterClassExA failed");

    auto window = CreateWindowExA(
        0,
        windowclass.lpszClassName,
        "AwesomeWindowTitle",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,  // x
        CW_USEDEFAULT,  // y
        CW_USEDEFAULT,  // w
        CW_USEDEFAULT,  // h
        0,
        0,
        instance,
        0
    );

    if (!window) Assert("CreateWindowEx failed");

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

    //
    // We'll allocate a backbuffer that has enough room for the maximum
    // possible window size, so we don't have to realloc on window size changes
    // @Incomplete: It's possible that the maximum changes
    //
    int max_w, max_h;
    query_max_possible_window_size(&max_w, &max_h);
    


    MSG message{};
    bool running = true;

    while (running) {

        auto getmsgres = GetMessageA(&message, 0, 0, 0);
        switch (getmsgres) {
            case 0:  // 0 = WM_QUIT ; -1 = Error
            case -1: {
                running = false;
            } break;

            default: {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
        }

    }

    return message.wParam;
}


LRESULT WINAPI win32_window_callback_proc(HWND window, UINT msg, WPARAM wp, LPARAM lp)
{
    LRESULT result = 0;

    switch (msg) {

        case WM_SIZE: {
            int new_w = LOWORD(lp), new_h = HIWORD(lp);
            result = 0;
        } break;

        case WM_DESTROY: {
            PostQuitMessage(0);
            result = 0;
        } break;

        default: {
            result = DefWindowProcA(window, msg, wp, lp);
        } break;
    }

    return result;
}
