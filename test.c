#define _USE_MATH_DEFINES
#define M_PI 3.14159265358979323846

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Function to test basic arithmetic
void test_arithmetic() {
    printf("Arithmetic Test:\n");
    int a = 10, b = 3;
    printf("  Addition: %d + %d = %d\n", a, b, a + b);
    printf("  Subtraction: %d - %d = %d\n", a, b, a - b);
    printf("  Multiplication: %d * %d = %d\n", a, b, a * b);
    printf("  Division: %d / %d = %d\n", a, b, a / b);
    printf("  Modulus: %d %% %d = %d\n", a, b, a % b);
}

// Function to test mathematical functions
void test_math_functions() {
    printf("\nMath Functions Test:\n");
    double x = 45.0;
    printf("  sin(45°): %.4f\n", sin(x * M_PI / 180.0));
    printf("  cos(45°): %.4f\n", cos(x * M_PI / 180.0));
    printf("  tan(45°): %.4f\n", tan(x * M_PI / 180.0));
    printf("  sqrt(16): %.4f\n", sqrt(16.0));
    printf("  pow(2, 3): %.4f\n", pow(2.0, 3.0));
}

// Function to test string manipulation
void test_string_functions() {
    printf("\nString Functions Test:\n");
    char str1[50] = "Hello";
    char str2[50] = " World";
    strcat(str1, str2);
    printf("  String concatenation: %s\n", str1);
    
    char buffer[100];
    int value = 42;
    sprintf(buffer, "The answer is %d", value);
    printf("  String formatting: %s\n", buffer);
}

// Function to test time functions
void test_time_functions() {
    printf("\nTime Functions Test:\n");
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    
    char time_buffer[50];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", local_time);
    printf("  Current local time: %s\n", time_buffer);
}

// Function to test dynamic memory allocation
void test_memory_allocation() {
    printf("\nMemory Allocation Test:\n");
    int* dynamic_array = (int*)malloc(5 * sizeof(int));
    
    if (dynamic_array == NULL) {
        printf("  Memory allocation failed!\n");
        return;
    }
    
    for (int i = 0; i < 5; i++) {
        dynamic_array[i] = i * 10;
    }
    
    printf("  Dynamic array contents:");
    for (int i = 0; i < 5; i++) {
        printf(" %d", dynamic_array[i]);
    }
    printf("\n");
    
    free(dynamic_array);
}

int main() {
    printf("Comprehensive C Environment Test\n");
    printf("--------------------------------\n");
    
    test_arithmetic();
    test_math_functions();
    test_string_functions();
    test_time_functions();
    test_memory_allocation();
    
    printf("\nAll tests completed successfully!\n");
    return 0;
}
