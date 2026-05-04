#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Verwaltung der Benutzereingabe und Anzeige von sortierten Namen.
 *
 * Diese Klasse implementiert die Hauptfenster-Logik für die Eingabe von Vor- und Nachnamen.
 * Die Namen werden sortiert nach Nachnamen in einem Container gespeichert und können
 * auf Anforderung zeilenweise im Format "Nachname, Vorname" ausgegeben werden.
 *
 * @author Henry Mirus
 * @date April 2026
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor für MainWindow.
     *
     * Initialisiert das Hauptfenster und verbindet das returnPressed()-Signal
     * des Eingabefeldes mit dem Slot handleReturnPressed().
     *
     * @param[in] parent Übergeordnetes Widget (Standard: nullptr).
     *
     * @author Henry Mirus
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor für MainWindow.
     *
     * Gibt die vom UI-Designer generierte Benutzeroberflächeninstanz frei.
     *
     * @author Henry Mirus
     */
    ~MainWindow() override;

private slots:
    /**
     * @brief Verarbeitet die Return/Enter-Taste im Eingabefeld.
     *
     * Wenn das Eingabefeld nicht leer ist: Parst die Eingabe als "Vorname Nachname",
     * validiert das Format (inkl. Bindestrich-Namen), speichert das Paar nach Nachnamen
     * sortiert ab und leert das Eingabefeld. Bei ungültiger Eingabe wird eine Warnung angezeigt.
     *
     * Wenn das Eingabefeld leer ist: Gibt alle gespeicherten Namen in einem neuen Fenster
     * (QMessageBox) zeilenweise im Format "Nachname, Vorname" aus.
     *
     * @return void
     *
     * @sideeffect Ändert den Inhalt der Container namesByLastName,
     *             leert das Eingabefeld, zeigt ein QMessageBox-Fenster.
     *
     * @author Henry Mirus
     */
    void handleReturnPressed();

private:
    Ui::MainWindow *ui;  ///< Zeiger auf die vom UI-Designer generierte Benutzeroberflächeninstanz.
    QMap<QString, QString> namesByLastName;  ///< Container: Schlüssel = Nachname, Wert = Vorname; automatisch nach Nachnamen sortiert.
};
#endif // MAINWINDOW_H
