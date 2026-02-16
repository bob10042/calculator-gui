#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define MAX_RANGE 1000

void find_primes(int n) {
    bool is_prime[MAX_RANGE + 1];
    
    // Initialize all numbers as prime
    for (int i = 0; i <= n; i++) {
        is_prime[i] = true;
    }
    
    // 0 and 1 are not prime
    is_prime[0] = is_prime[1] = false;
    
    // Sieve of Eratosthenes algorithm
    for (int p = 2; p * p <= n; p++) {
        if (is_prime[p]) {
            // Mark multiples of p as not prime
            for (int i = p * p; i <= n; i += p) {
                is_prime[i] = false;
            }
        }
    }
    
    // Print prime numbers
    printf("Prime numbers up to %d are:\n", n);
    for (int p = 2; p <= n; p++) {
        if (is_prime[p]) {
            printf("%d ", p);
        }
    }
    printf("\n");
}

int main() {
    int range;
    
    printf("Enter the range to find prime numbers (max %d): ", MAX_RANGE);
    scanf("%d", &range);
    
    // Validate input
    if (range < 2 || range > MAX_RANGE) {
        printf("Invalid range. Please enter a number between 2 and %d.\n", MAX_RANGE);
        return 1;
    }
    
    find_primes(range);
    
    return 0;
}
