#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QStringList>
#include <deque>
#include <string>

/**
 * Autor: Henry Mirus
 * Zweck: Diese Klasse stellt das Hauptfenster von Aufgabe 4 bereit.
 * Sie liest eine komma-getrennte Woerterliste ein, ordnet die Woerter
 * abwechselnd vorne und hinten ein und zeigt das Ergebnis an.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Autor: Henry Mirus
     * Eingabeparameter: parent - optionales Eltern-Widget.
     * Rueckgabeparameter: keiner.
     * Seiteneffekte: Initialisiert Widgets, Layout und Signal-Slot-Verbindungen.
     */
    MainWindow(QWidget* parent = nullptr);

    /**
     * Autor: Henry Mirus
     * Eingabeparameter: keine.
     * Rueckgabeparameter: keiner.
     * Seiteneffekte: Gibt Ressourcen des Fensters frei.
     */
    ~MainWindow();

private slots:
    /**
     * Autor: Henry Mirus
     * Eingabeparameter: keine.
     * Rueckgabeparameter: keiner.
     * Seiteneffekte: Liest Eingabe aus dem Feld, verarbeitet sie und aktualisiert die Ausgabe.
     */
    void onInputReturn();

private:
    /**
     * Autor: Henry Mirus
     * Eingabeparameter: input - komma-getrennte Eingabezeichenkette.
     * Rueckgabeparameter: keiner.
     * Seiteneffekte: Fuellt die interne Ergebnisliste neu.
     */
    void processInput(const std::string& input);

    /**
     * Autor: Henry Mirus
     * Eingabeparameter: keine.
     * Rueckgabeparameter: keiner.
     * Seiteneffekte: Schreibt die aktuelle Liste in den Ausgabebereich.
     */
    void displayResult();

    QLineEdit* inputField;
    QTextEdit* outputField;
    std::deque<std::string> resultList;
};

#endif // MAINWINDOW_H
