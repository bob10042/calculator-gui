#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <windows.h>

// Define the IDC_EDIT constant
#define IDC_EDIT 1000

// Calculator class definition
class Calculator {
public:
    Calculator();
    ~Calculator();

    void init(HWND hWnd);
    void handleEvent(UINT message, WPARAM wParam, LPARAM lParam);
    void calculate();

private:
    HWND hWnd;
    int num1;
    int num2;
    int op;
};

#endif // CALCULATOR_H
