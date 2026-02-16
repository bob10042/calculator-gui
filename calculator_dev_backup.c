#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define the window class and title
#define WINDOW_CLASS "Calculator"
#define WINDOW_TITLE "Scientific Calculator"

// Define the window size
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

// Button IDs
#define ID_ADD 101
#define ID_SUB 102
#define ID_MUL 103
#define ID_DIV 104
#define ID_EQ  105
#define ID_SIN 106
#define ID_COS 107
#define ID_TAN 108
#define ID_SQRT 109
#define ID_POW 110
#define ID_MEM 111

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Failed to register window class!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    // Create the window
    HWND hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, "Failed to create window!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Run the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    //... (rest of the code remains the same)
}
