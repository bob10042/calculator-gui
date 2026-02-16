#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Verbose mode for detailed sorting steps
#define VERBOSE 0

// Generic swap macro for any type
#define SWAP(type, a, b) \
    do { \
        type temp = (a); \
        (a) = (b); \
        (b) = temp; \
    } while(0)

// Print array with optional message
void printArray(int arr[], int size, const char* message, bool force_print) {
    #if VERBOSE
    force_print = true;
    #endif

    if (force_print) {
        printf("%s: ", message ? message : "Array");
        for (int i = 0; i < size; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }
}

// Partition function with improved error handling
int partition(int arr[], int low, int high) {
    // Validate input
    if (low >= high) return low;

    int pivot = arr[high];
    printArray(arr + low, high - low + 1, "Current subarray", false);
    
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            SWAP(int, arr[i], arr[j]);
            printArray(arr + low, high - low + 1, "After swap", false);
        }
    }

    SWAP(int, arr[i + 1], arr[high]);
    printArray(arr + low, high - low + 1, "After partition", false);
    return i + 1;
}

// Quicksort with improved error handling and optional recursion limit
void quicksort(int arr[], int low, int high, int depth_limit) {
    // Prevent excessive recursion
    if (depth_limit <= 0) {
        // Fallback to insertion sort for small subarrays
        for (int i = low + 1; i <= high; i++) {
            int key = arr[i];
            int j = i - 1;
            while (j >= low && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
        return;
    }

    if (low < high) {
        #if VERBOSE
        printf("\nQuicksorting subarray from index %d to %d\n", low, high);
        #endif

        int pivot = partition(arr, low, high);

        // Recursive calls with reduced depth limit
        quicksort(arr, low, pivot - 1, depth_limit - 1);
        quicksort(arr, pivot + 1, high, depth_limit - 1);
    }
}

// Main function demonstrating quicksort usage
int main() {
    int arr[] = {5, 2, 9, 1, 7, 3, 8, 4, 6};
    int n = sizeof(arr) / sizeof(arr[0]);

    // Calculate depth limit to prevent worst-case scenarios
    int depth_limit = 2 * (int)(sizeof(int) * 8 - __builtin_clz(n));

    printf("Original array: ");
    printArray(arr, n, "Before sorting", true);

    // Sort the array
    quicksort(arr, 0, n - 1, depth_limit);

    printf("\nFinal sorted array: ");
    printArray(arr, n, "After sorting", true);

    return 0;
}
