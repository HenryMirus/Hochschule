#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <climits>
#include <cstring>

#include "fak.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Fehler: Keine Zahl angegeben.\n";
        std::cerr << "Verwendung: " << argv[0] << " <n>\n";
        return 1;
    }

    if (argc > 2) {
        std::cerr << "Fehler: Zu viele Argumente.\n";
        std::cerr << "Verwendung: " << argv[0] << " <n>\n";
        return 1;
    }

    char* endptr;
    long n = std::strtol(argv[1], &endptr, 10);

    if (endptr == argv[1] || *endptr != '\0') {
        std::cerr << "Fehler: '" << argv[1] << "' ist keine gueltige ganze Zahl.\n";
        return 1;
    }

    if (n < 0) {
        std::cerr << "Fehler: Fakultaet ist nur fuer nicht-negative Zahlen definiert.\n";
        return 1;
    }

    if (n > 20) {
        std::cerr << "Fehler: Fakultaet von " << n << " ueberschreitet den Bereich von long long.\n";
        std::cerr << "Maximum: 20\n";
        return 1;
    }

    std::cout << fak(static_cast<int>(n)) << '\n';
    return 0;
}
