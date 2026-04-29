#include <QApplication>
#include "MainWindow.h"

/**
 * Autor: Henry Mirus
 * Eingabeparameter: argc, argv - Programmargumente.
 * Rueckgabeparameter: Exit-Code der Qt-Applikation.
 * Seiteneffekte: Startet die GUI und den Event-Loop.
 */
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
