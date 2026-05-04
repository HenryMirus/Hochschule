#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Hauptfenster der Anwendung zur Verarbeitung komma-separierter Worteingaben.
 *
 * Autor: Henry Mirus
 *
 * Zweck der Klasse:
 * - Entgegennahme einer durch Kommata getrennten Eingabe aus einem QLineEdit.
 * - Verarbeitung der Eingabe nach dem Muster vorne/hinten/vorne/hinten.
 * - Ausgabe der berechneten Reihenfolge in einem QListWidget.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor des Hauptfensters.
     *
     * Autor: Henry Mirus
     *
     * @param parent Zeiger auf das übergeordnete Widget. Standard ist nullptr.
     * @return Keine Rückgabe (Konstruktor).
     * @sideeffects Initialisiert die UI-Komponenten und registriert Signal-Slot-Verbindungen.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor des Hauptfensters.
     *
     * Autor: Henry Mirus
     *
     * @return Keine Rückgabe (Destruktor).
     * @sideeffects Gibt die von der UI belegten Ressourcen frei.
     */
    ~MainWindow() override;

private:
    /**
     * @brief Verarbeitet die Eingabe aus dem Eingabefeld und aktualisiert die Ausgabe.
     *
     * Autor: Henry Mirus
     *
     * Eingabe:
     * - Keine direkten Parameter. Die Funktion liest den Text aus ui->lineEdit.
     *
     * Rückgabe:
     * - Keine (void).
     *
     * Seiteneffekte:
     * - Liest den aktuellen Text aus dem Eingabefeld.
     * - Zerlegt den Text anhand von Kommata in einzelne Wörter.
     * - Fügt bereinigte Wörter abwechselnd vorne und hinten in eine Ergebnisliste ein.
     * - Löscht den bisherigen Inhalt des Ausgabebereichs (QListWidget).
     * - Schreibt die neue Reihenfolge in den Ausgabebereich.
     */
    void processCommaSeparatedInput();

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
