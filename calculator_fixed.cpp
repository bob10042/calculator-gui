#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //...
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    //...
}

HWND hwndEdit = CreateWindowEx(0, TEXT("EDIT"), TEXT("Calculator"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, 10, 10, 200, 20, hwnd, (HMENU)1, hInstance, NULL);

HWND hwnd = CreateWindowEx(0, TEXT("STATIC"), TEXT("Calculator"), WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 10, 200, 20, NULL, NULL, hInstance, NULL);
HINSTANCE hInstance = GetModuleHandle(NULL);
