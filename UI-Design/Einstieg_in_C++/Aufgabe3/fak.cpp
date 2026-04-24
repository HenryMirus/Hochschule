#include "fak.h"

long long fak(int n)
{
    if (n < 0) {
        return -1;
    }

    if (n == 0 || n == 1) {
        return 1;
    }

    long long result = 1;

    for (int i = 2; i <= n; ++i) {
        result *= i;
    }

    return result;
}