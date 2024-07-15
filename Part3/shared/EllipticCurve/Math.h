#pragma once
#include <Arduino.h>

// Function to calculate gcd(a, b) using Euclidean algorithm
inline int gcdExtended(int a, int b, int *x, int *y) {
    // Base case
    if (a == 0) {
        *x = 0, *y = 1;
        return b;
    }

    int x1, y1; // To store results of recursive call
    int gcd = gcdExtended(b % a, a, &x1, &y1);

    // Update x and y using results of recursive call
    *x = y1 - (b / a) * x1;
    *y = x1;

    return gcd;
}

// Function to find modular inverse of a under regular_modulo p
inline int modularInverse(int a, int p) {
    int x, y;
    int gcd = gcdExtended(a, p, &x, &y);

    if (gcd != 1) {
        // Modular inverse doesn't exist
        return -1;
    } else {
        // Handling negative x to ensure it's positive
        return (x % p + p) % p;
    }
}

inline uint32_t module(int32_t a, int32_t b) {
    int r = a % b;
    if (r < 0)
        r += b;
    return r;
}