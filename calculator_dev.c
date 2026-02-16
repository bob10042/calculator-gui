#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define the window class and title
#define WINDOW_CLASS L"Calculator"
#define WINDOW_TITLE L"Scientific Calculator"

// Define the window size
#define WINDOW_WIDTH 500  // Increased width for new buttons
#define WINDOW_HEIGHT 400
#define MIN_WIDTH 500
#define MIN_HEIGHT 400

// Max terms in polynomial
#define MAX_TERMS 10

// Button IDs
#define ID_NUM_0 100
#define ID_NUM_1 101
#define ID_NUM_2 102
#define ID_NUM_3 103
#define ID_NUM_4 104
#define ID_NUM_5 105
#define ID_NUM_6 106
#define ID_NUM_7 107
#define ID_NUM_8 108
#define ID_NUM_9 109
#define ID_DOT 110
#define ID_ADD 111
#define ID_SUB 112
#define ID_MUL 113
#define ID_DIV 114
#define ID_EQ  115
#define ID_SIN 116
#define ID_COS 117
#define ID_TAN 118
#define ID_SQRT 119
#define ID_POW 120
#define ID_MC 121
#define ID_MR 122
#define ID_MP 123
#define ID_CLR 124
#define ID_INPUT 125
#define ID_DIFF 126  // New differentiation button
#define ID_INT 127   // New integration button
#define ID_X 128     // New X variable button

// Custom colors
#define BG_COLOR RGB(45, 45, 45)
#define DISPLAY_COLOR RGB(30, 30, 30)
#define NUMBER_COLOR RGB(60, 60, 60)
#define OPERATOR_COLOR RGB(255, 149, 0)
#define FUNCTION_COLOR RGB(80, 80, 80)
#define MEMORY_COLOR RGB(70, 70, 70)
#define EQUALS_COLOR RGB(0, 122, 255)
#define CLEAR_COLOR RGB(255, 59, 48)
#define TEXT_COLOR RGB(255, 255, 255)
#define CALCULUS_COLOR RGB(0, 200, 83)  // Green color for calculus functions

// Function declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Polynomial term structure
typedef struct {
    double coefficient;
    int exponent;
} Term;

// Global variables
double memory = 0.0;
double currentValue = 0.0;
double previousValue = 0.0;
int currentOperation = 0;
HWND hInput;
HFONT hFont, hFontBold;
HWND hButtons[30];  // Increased size for new buttons
WNDPROC oldButtonProc;
HBRUSH hbrBackground;
HBRUSH hbrDisplay;
HBRUSH hbrNumber;
HBRUSH hbrOperator;
HBRUSH hbrFunction;
HBRUSH hbrMemory;
HBRUSH hbrEquals;
HBRUSH hbrClear;
HBRUSH hbrCalculus;

// Function to parse polynomial string into terms
int parsePolynomial(const wchar_t* input, Term* terms) {
    int termCount = 0;
    wchar_t* str = _wcsdup(input);
    wchar_t* token = wcstok(str, L"+", NULL);
    
    while (token && termCount < MAX_TERMS) {
        Term* term = &terms[termCount];
        wchar_t* xPos = wcschr(token, L'x');
        
        if (xPos) {
            // Term has x
            wchar_t* expPos = wcschr(xPos, L'^');
            if (expPos) {
                // Term has exponent
                *xPos = L'\0';
                term->exponent = _wtoi(expPos + 1);
            } else {
                // x without exponent
                *xPos = L'\0';
                term->exponent = 1;
            }
            term->coefficient = wcslen(token) ? _wtof(token) : 1.0;
        } else {
            // Constant term
            term->coefficient = _wtof(token);
            term->exponent = 0;
        }
        termCount++;
        token = wcstok(NULL, L"+", NULL);
    }
    
    free(str);
    return termCount;
}

// Function to differentiate polynomial
void differentiate(Term* terms, int termCount) {
    for (int i = 0; i < termCount; i++) {
        if (terms[i].exponent > 0) {
            terms[i].coefficient *= terms[i].exponent;
            terms[i].exponent--;
        } else {
            terms[i].coefficient = 0;
        }
    }
}

// Function to integrate polynomial
void integrate(Term* terms, int termCount) {
    for (int i = 0; i < termCount; i++) {
        terms[i].exponent++;
        terms[i].coefficient /= terms[i].exponent;
    }
}

// Function to convert terms back to string
void termsToString(Term* terms, int termCount, wchar_t* output, size_t outputSize) {
    output[0] = L'\0';
    for (int i = 0; i < termCount; i++) {
        wchar_t term[64];
        if (terms[i].coefficient == 0) continue;
        
        if (i > 0 && terms[i].coefficient > 0) wcscat_s(output, outputSize, L"+");
        
        if (terms[i].exponent == 0) {
            swprintf(term, 64, L"%.2f", terms[i].coefficient);
        } else if (terms[i].exponent == 1) {
            swprintf(term, 64, L"%.2fx", terms[i].coefficient);
        } else {
            swprintf(term, 64, L"%.2fx^%d", terms[i].coefficient, terms[i].exponent);
        }
        wcscat_s(output, outputSize, term);
    }
    
    if (output[0] == L'\0') wcscpy_s(output, outputSize, L"0");
}

void AppendNumber(HWND hInput, const wchar_t* number) {
    wchar_t current[256];
    GetWindowTextW(hInput, current, 256);
    if (wcscmp(current, L"0") == 0 && wcscmp(number, L".") != 0) {
        SetWindowTextW(hInput, number);
    } else {
        wcscat_s(current, 256, number);
        SetWindowTextW(hInput, current);
    }
}

COLORREF LightenColor(COLORREF color, int amount) {
    int r = GetRValue(color) + amount;
    int g = GetGValue(color) + amount;
    int b = GetBValue(color) + amount;
    r = r > 255 ? 255 : r;
    g = g > 255 ? 255 : g;
    b = b > 255 ? 255 : b;
    return RGB(r, g, b);
}

// ... [Previous WinMain function remains the same until button creation] ...

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            // Create input field with larger font
            RECT rect;
            GetClientRect(hWnd, &rect);
            hInput = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"0",
                WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,
                10, 10, WINDOW_WIDTH-40, 40,
                hWnd, (HMENU)(intptr_t)ID_INPUT, GetModuleHandle(NULL), NULL);
            SendMessage(hInput, WM_SETFONT, (WPARAM)hFontBold, TRUE);

            // Create brushes
            hbrCalculus = CreateSolidBrush(CALCULUS_COLOR);

            // Button configuration with colors
            struct {
                int id;
                const wchar_t* label;
                int row;
                int col;
                HBRUSH brush;
            } buttons[] = {
                {ID_NUM_7, L"7", 1, 0, hbrNumber}, {ID_NUM_8, L"8", 1, 1, hbrNumber}, {ID_NUM_9, L"9", 1, 2, hbrNumber},
                {ID_ADD, L"+", 1, 3, hbrOperator}, {ID_SIN, L"sin", 1, 4, hbrFunction}, {ID_DIFF, L"d/dx", 1, 5, hbrCalculus},
                {ID_NUM_4, L"4", 2, 0, hbrNumber}, {ID_NUM_5, L"5", 2, 1, hbrNumber}, {ID_NUM_6, L"6", 2, 2, hbrNumber},
                {ID_SUB, L"-", 2, 3, hbrOperator}, {ID_COS, L"cos", 2, 4, hbrFunction}, {ID_INT, L"∫", 2, 5, hbrCalculus},
                {ID_NUM_1, L"1", 3, 0, hbrNumber}, {ID_NUM_2, L"2", 3, 1, hbrNumber}, {ID_NUM_3, L"3", 3, 2, hbrNumber},
                {ID_MUL, L"×", 3, 3, hbrOperator}, {ID_TAN, L"tan", 3, 4, hbrFunction}, {ID_X, L"x", 3, 5, hbrCalculus},
                {ID_NUM_0, L"0", 4, 0, hbrNumber}, {ID_DOT, L".", 4, 1, hbrNumber}, {ID_EQ, L"=", 4, 2, hbrEquals},
                {ID_DIV, L"÷", 4, 3, hbrOperator}, {ID_SQRT, L"√", 4, 4, hbrFunction}, {ID_POW, L"^", 4, 5, hbrOperator},
                {ID_MC, L"MC", 5, 0, hbrMemory}, {ID_MR, L"MR", 5, 1, hbrMemory}, {ID_MP, L"M+", 5, 2, hbrMemory},
                {ID_CLR, L"C", 5, 3, hbrClear}
            };
            
            int buttonWidth = 70;
            int buttonHeight = 50;
            int startX = 10;
            int startY = 60;
            
            for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
                hButtons[i] = CreateWindowExW(0, L"BUTTON", buttons[i].label,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
                    startX + buttons[i].col * (buttonWidth + 5),
                    startY + buttons[i].row * (buttonHeight + 5),
                    buttonWidth, buttonHeight,
                    hWnd, (HMENU)(intptr_t)buttons[i].id, GetModuleHandle(NULL), NULL);
                
                SetWindowLongPtr(hButtons[i], GWLP_USERDATA, (LONG_PTR)buttons[i].brush);
                SendMessage(hButtons[i], WM_SETFONT, (WPARAM)hFont, TRUE);
                oldButtonProc = (WNDPROC)SetWindowLongPtr(hButtons[i], GWLP_WNDPROC, (LONG_PTR)ButtonProc);
            }
            break;
        }

        // ... [Previous message handling remains the same] ...

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            wchar_t buffer[256];

            switch (wmId) {
                // ... [Previous command handling remains the same] ...

                case ID_DIFF: {
                    GetWindowTextW(hInput, buffer, 256);
                    Term terms[MAX_TERMS];
                    int termCount = parsePolynomial(buffer, terms);
                    differentiate(terms, termCount);
                    termsToString(terms, termCount, buffer, 256);
                    SetWindowTextW(hInput, buffer);
                    break;
                }

                case ID_INT: {
                    GetWindowTextW(hInput, buffer, 256);
                    Term terms[MAX_TERMS];
                    int termCount = parsePolynomial(buffer, terms);
                    integrate(terms, termCount);
                    termsToString(terms, termCount, buffer, 256);
                    wcscat_s(buffer, 256, L"+C");  // Add constant of integration
                    SetWindowTextW(hInput, buffer);
                    break;
                }

                case ID_X:
                    AppendNumber(hInput, L"x");
                    break;

                // ... [Rest of the command handling remains the same] ...
            }
            break;
        }

        // ... [Rest of the window procedure remains the same] ...
    }
    return 0;
}

// ... [Rest of the code remains the same] ...
