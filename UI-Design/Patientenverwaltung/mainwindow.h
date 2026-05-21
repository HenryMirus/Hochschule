#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QString>

#include "patientapi.h"
#include "patientjsonstorage.h"
#include "patienttablemodel.h"
#include "patienteditdialog.h"
#include "patientviewdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// Autor: Henry Mirus
// Zweck: Hauptfenster der Patientenverwaltung mit Liste, Suche und CRUD-
// Aktionen für die Patienten.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Autor: Henry Mirus
    // Eingabe: optionales Elternobjekt.
    // Rückgabe: keine.
    // Seiteneffekte: Initialisiert das Hauptfenster und lädt die Daten.
    explicit MainWindow(QWidget *parent = nullptr);

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Gibt interne Qt-Ressourcen frei.
    ~MainWindow() override;

private slots:
    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Öffnet den Anzeigedialog für den selektierten Patienten.
    void on_ViewButton_clicked();

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Öffnet den Bearbeitungsdialog für einen neuen Patienten.
    void on_AddButton_clicked();

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Aktualisiert die Tabelle anhand des Suchfeldes.
    void on_SearchButton_clicked();

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Öffnet den Bearbeitungsdialog für den selektierten Patienten.
    void on_EditButton_clicked();

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Löscht den selektierten Patienten nach Sicherheitsabfrage.
    void on_DeleteButton_clicked();

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Hebt die aktuelle Tabellen-Auswahl auf.
    void on_ClearSelectionButton_clicked();

private:
    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: `true`, wenn eine Tabellenzeile ausgewählt ist.
    // Seiteneffekte: keine.
    bool hasSelectedPatientRow() const;

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: Zeilenindex der selektierten Patientenquelle oder `-1`.
    // Seiteneffekte: keine.
    int selectedSourceIndex() const;

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Baut die sichtbare Tabellenliste aus der aktuellen Suche neu auf.
    void reloadTable();

    Ui::MainWindow *ui;

    // Datenhaltung und Anzeige
    PatientApi m_patientApi;
    PatientTableModel *m_model = nullptr;

    // Mapping der aktuell angezeigten Tabellenzeilen auf Indizes der API-Liste.
    // Wichtig für Edit/Delete bei aktiver Suche.
    QList<int> m_rowToSourceIndex;

    // Speicherort der JSON-Datei, relativ zum Programmverzeichnis.
    QString m_defaultDataFile;
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
