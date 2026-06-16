#include "mainwindow.h"

#include <QApplication>

/*
 * Autor: Henry Mirus
 * Eingabe: argc – Anzahl der Kommandozeilenargumente
 *          argv – Kommandozeilenargumente
 * Rückgabe: Exit-Code der Anwendung
 * Seiteneffekte: Erzeugt QApplication und MainWindow, zeigt das Hauptfenster
 *                an und übergibt die Kontrolle an die Qt-Ereignisschleife.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QStringLiteral("Tuerme von Hanoi"));
    a.setApplicationVersion(QStringLiteral("1.0"));

    MainWindow w;
    w.show();

    return a.exec();
}
