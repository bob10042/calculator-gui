#include <windows.h>
#include <math.h>
#include <stdio.h>

// Define the window class and title
#define WINDOW_CLASS "Calculator"
#define WINDOW_TITLE "Scientific Calculator"

// Define the window size
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 200

// Function prototype for WndProc
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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

    // Add a loop to keep the program running
    int response;
    printf("Do you want to exit the program? (1 for yes, 0 for no): ");
    scanf("%d", &response);
    while (response != 1) {
        printf("Do you want to exit the program? (1 for yes, 0 for no): ");
        scanf("%d", &response);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit;
    static double num1, num2;
    static char op;

    switch (message) {
        case WM_CREATE:
            hwndEdit = CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 10, 260, 20, hWnd, NULL, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "0", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 40, 40, 20, hWnd, (HMENU)0, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "1", WS_CHILD | WS_VISIBLE | WS_BORDER, 60, 40, 40, 20, hWnd, (HMENU)1, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "2", WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 40, 40, 20, hWnd, (HMENU)2, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "3", WS_CHILD | WS_VISIBLE | WS_BORDER, 160, 40, 40, 20, hWnd, (HMENU)3, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "4", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 70, 40, 20, hWnd, (HMENU)4, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "5", WS_CHILD | WS_VISIBLE | WS_BORDER, 60, 70, 40, 20, hWnd, (HMENU)5, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "6", WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 70, 40, 20, hWnd, (HMENU)6, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "7", WS_CHILD | WS_VISIBLE | WS_BORDER, 160, 70, 40, 20, hWnd, (HMENU)7, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "8", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 100, 40, 20, hWnd, (HMENU)8, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "9", WS_CHILD | WS_VISIBLE | WS_BORDER, 60, 100, 40, 20, hWnd, (HMENU)9, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "10", WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 100, 40, 20, hWnd, (HMENU)10, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "+", WS_CHILD | WS_VISIBLE | WS_BORDER, 160, 100, 40, 20, hWnd, (HMENU)11, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "-", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 130, 40, 20, hWnd, (HMENU)12, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "*", WS_CHILD | WS_VISIBLE | WS_BORDER, 60, 130, 40, 20, hWnd, (HMENU)13, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "/", WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 130, 40, 20, hWnd, (HMENU)14, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "=", WS_CHILD | WS_VISIBLE | WS_BORDER, 160, 130, 40, 20, hWnd, (HMENU)15, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "C", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 160, 40, 20, hWnd, (HMENU)16, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "sin", WS_CHILD | WS_VISIBLE | WS_BORDER, 60, 160, 40, 20, hWnd, (HMENU)17, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "cos", WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 160, 40, 20, hWnd, (HMENU)18, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "tan", WS_CHILD | WS_VISIBLE | WS_BORDER, 160, 160, 40, 20, hWnd, (HMENU)19, NULL, NULL);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    SendMessage(hwndEdit, EM_SETSEL, -1, -1);
                    SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("0123456789") + LOWORD(wParam));
                    break;
                case 10:
                case 11:
                case 12:
                case 13:
                    num1 = GetDlgItemInt(hWnd, (int)(intptr_t)hwndEdit, NULL, FALSE);
                    op = (char)LOWORD(wParam);
                    SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)"");
                    break;
                case 14:
                    num2 = GetDlgItemInt(hWnd, (int)(intptr_t)hwndEdit, NULL, FALSE);
                    switch (op) {
                        case 10:
                            num1 += num2;
                            break;
                        case 11:
                            num1 -= num2;
                            break;
                        case 12:
                            num1 *= num2;
                            break;
                        case 13:
                            num1 /= num2;
                            break;
                    }
                    char result[20];
                    sprintf(result, "%.2f", num1);
                    SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)result);
                    break;
                case 15:
                case 16:
                case 17:
                    num1 = GetDlgItemInt(hWnd, (int)(intptr_t)hwndEdit, NULL, FALSE);
                    switch (LOWORD(wParam)) {
                        case 15:
                            num1 = sin(num1);
                            break;
                        case 16:
                            num1 = cos(num1);
                            break;
                        case 17:
                            num1 = tan(num1);
                            break;
                    }
                    sprintf(result, "%.2f", num1);
                    SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)result);
                    break;
            }
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
