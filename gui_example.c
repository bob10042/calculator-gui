#include <windows.h>

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Button clicked
                MessageBox(hwnd, "Hello! You clicked the button!", "Button Click", MB_OK | MB_ICONINFORMATION);
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    // Window Class
    WNDCLASSW wc = {0};
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"MyWindowClass";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClassW(&wc))
        return -1;

    // Create Window
    HWND hwnd = CreateWindowW(L"MyWindowClass", L"My GUI Window",
                 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                 100, 100, 500, 400,
                 NULL, NULL, hInst, NULL);

    if (hwnd == NULL) {
        return -1;
    }

    // Create Button
    CreateWindowW(L"Button", L"Click Me!",
                 WS_VISIBLE | WS_CHILD,
                 200, 150, 100, 30,
                 hwnd, (HMENU)1, hInst, NULL);

    // Message Loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
