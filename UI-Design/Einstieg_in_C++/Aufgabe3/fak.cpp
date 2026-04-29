#include "fak.h"

/**
 * Autor: Henry Mirus
 * Eingabeparameter: n - nicht-negative ganze Zahl.
 * Rueckgabeparameter: Fakultaet von n als long long; bei n < 0 wird -1 zurueckgegeben.
 * Seiteneffekte: keine.
 */
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