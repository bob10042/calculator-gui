#define _USE_MATH_DEFINES
#define M_PI 3.14159265358979323846

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_CLASS "Calculator"
#define WINDOW_TITLE "Scientific Calculator"
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

// Button ID definitions to avoid direct casting
#define ID_BUTTON_BASE 200
#define ID_OPERATION_BASE 250

// Function prototype for WndProc
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = WINDOW_CLASS;

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONERROR | MB_OK);
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
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit;
    static double num1 = 0, num2 = 0;
    static char op = '\0';
    static char buffer[256];

    switch (message) {
        case WM_CREATE: {
            // Create edit control for display
            hwndEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE, "EDIT", "", 
                WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,
                10, 10, 370, 30, 
                hWnd, (HMENU)100, GetModuleHandle(NULL), NULL);

            // Number buttons
            int buttons[] = {7, 8, 9, 4, 5, 6, 1, 2, 3, 0};
            int x, y;
            for (int i = 0; i < 10; i++) {
                x = 10 + (i % 3) * 60;
                y = 50 + (i / 3) * 50;
                char digit_str[2];
                sprintf(digit_str, "%d", buttons[i]);
                CreateWindowEx(0, "BUTTON", 
                    digit_str, 
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    x, y, 50, 40, 
                    hWnd, (HMENU)(UINT_PTR)(ID_BUTTON_BASE + buttons[i]), 
                    GetModuleHandle(NULL), NULL);
            }

            // Operation buttons
            char* ops[] = {"+", "-", "*", "/", "sin", "cos", "tan", "="};
            for (int i = 0; i < 8; i++) {
                x = 190 + (i % 2) * 60;
                y = 50 + (i / 2) * 50;
                CreateWindowEx(0, "BUTTON", 
                    ops[i], 
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    x, y, 50, 40, 
                    hWnd, (HMENU)(UINT_PTR)(ID_OPERATION_BASE + i), 
                    GetModuleHandle(NULL), NULL);
            }
            break;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);

            // Number buttons
            if (wmId >= ID_BUTTON_BASE && wmId < ID_BUTTON_BASE + 10) {
                int digit = wmId - ID_BUTTON_BASE;
                GetWindowText(hwndEdit, buffer, sizeof(buffer));
                sprintf(buffer + strlen(buffer), "%d", digit);
                SetWindowText(hwndEdit, buffer);
            }

            // Operation buttons
            switch (wmId) {
                case ID_OPERATION_BASE: // +
                case ID_OPERATION_BASE + 1: // -
                case ID_OPERATION_BASE + 2: // *
                case ID_OPERATION_BASE + 3: // /
                    GetWindowText(hwndEdit, buffer, sizeof(buffer));
                    num1 = atof(buffer);
                    op = "+-*/"[wmId - ID_OPERATION_BASE];
                    SetWindowText(hwndEdit, "");
                    break;

                case ID_OPERATION_BASE + 4: // sin
                case ID_OPERATION_BASE + 5: // cos
                case ID_OPERATION_BASE + 6: // tan
                    GetWindowText(hwndEdit, buffer, sizeof(buffer));
                    num1 = atof(buffer);
                    double result = 0;
                    switch (wmId) {
                        case ID_OPERATION_BASE + 4: result = sin(num1 * 3.14159265358979323846 / 180.0); break;
                        case ID_OPERATION_BASE + 5: result = cos(num1 * 3.14159265358979323846 / 180.0); break;
                        case ID_OPERATION_BASE + 6: result = tan(num1 * 3.14159265358979323846 / 180.0); break;
                    }
                    sprintf(buffer, "%.4f", result);
                    SetWindowText(hwndEdit, buffer);
                    break;

                case ID_OPERATION_BASE + 7: // =
                    GetWindowText(hwndEdit, buffer, sizeof(buffer));
                    num2 = atof(buffer);
                    switch (op) {
                        case '+': num1 += num2; break;
                        case '-': num1 -= num2; break;
                        case '*': num1 *= num2; break;
                        case '/': 
                            if (num2 != 0) {
                                num1 /= num2; 
                            } else {
                                MessageBox(hWnd, "Division by Zero!", "Error", MB_ICONERROR | MB_OK);
                                num1 = 0;
                            }
                            break;
                    }
                    sprintf(buffer, "%.4f", num1);
                    SetWindowText(hwndEdit, buffer);
                    break;
            }
            break;
        }

        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
