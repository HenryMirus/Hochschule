#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QRegularExpression>

/**
 * MainWindow::MainWindow(QWidget *parent)
 *
 * Konstruktor: Initialisiert das Hauptfenster, lädt die UI-Datei und verbindet
 * das returnPressed-Signal des Eingabefeldes mit dem handleReturnPressed-Slot.
 *
 * @param[in] parent Übergeordnetes Widget (Standard: nullptr).
 *
 * @author Henry Mirus
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::handleReturnPressed);
}

/**
 * MainWindow::~MainWindow()
 *
 * Destruktor: Gibt die Benutzeroberflächeninstanz frei.
 *
 * @author Henry Mirus
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * MainWindow::handleReturnPressed()
 *
 * Slot, der aufgerufen wird, wenn der Benutzer im Eingabefeld die Return/Enter-Taste drückt.
 *
 * Verarbeitung:
 * - Wenn Eingabefeld nicht leer:
 *   1. Liest den Text und trimmt Leerzeichen.
 *   2. Prüft Format: "Vorname Nachname" (beide Teile können Bindestriche enthalten, z.B. Anna-Lena Mueller).
 *   3. Bei korrektem Format: Speichert das Paar in namesByLastName (nach Nachnamen sortiert).
 *   4. Bei ungültigem Format: Zeigt eine Fehlermeldung und bricht ab.
 *   5. Leert anschließend das Eingabefeld.
 *
 * - Wenn Eingabefeld leer:
 *   Erzeugt ein neues Fenster (QMessageBox) und gibt alle gespeicherten Namen
 *   zeilenweise im Format "Nachname, Vorname" aus.
 *
 * @return void
 *
 * @sideeffect
 *   - Ändert namesByLastName (fügt ein Paar hinzu oder wird nicht verändert).
 *   - Leert das Eingabefeld.
 *   - Zeigt eventuell ein QMessageBox-Fenster.
 *
 * @author Henry Mirus
 */
void MainWindow::handleReturnPressed()
{
    // Liest den aktuellen Text aus dem Eingabefeld und entfernt führende/nachfolgende Leerzeichen.
    const QString input = ui->lineEdit->text().trimmed();

    // Wenn Eingabefeld leer: Zeige alle gespeicherten Namen in QMessageBox.
    if (input.isEmpty()) {
        QStringList lines;
        // Iteriert über alle Einträge in namesByLastName (automatisch nach Nachname sortiert).
        for (auto it = namesByLastName.cbegin(); it != namesByLastName.cend(); ++it) {
            // Formatiert jeden Eintrag als "Nachname, Vorname".
            lines << QString("%1, %2").arg(it.key(), it.value());
        }

        // Erzeugt und zeigt das Info-Fenster.
        QMessageBox box(this);
        box.setWindowTitle("Sortierte Namen");
        box.setIcon(QMessageBox::Information);
        box.setText(lines.isEmpty() ? "Keine Namen vorhanden." : lines.join('\n'));
        box.exec();
        return;
    }

    // Regex-Muster zur Validierung: "Vorname Nachname"
    // Erlaubt: Einzelne oder Bindestrich-verbundene Namen (z.B. Anna-Lena)
    static const QRegularExpression pattern(
        "^([\\p{L}]+(?:-[\\p{L}]+)?)\\s+([\\p{L}]+(?:-[\\p{L}]+)?)$"
    );

    // Prüft, ob die Eingabe dem Muster entspricht.
    const QRegularExpressionMatch match = pattern.match(input);
    if (!match.hasMatch()) {
        // Bei Fehler: Zeige Warnmeldung und kehre ohne Speicherung zurück.
        QMessageBox::warning(
            this,
            "Ungueltige Eingabe",
            "Bitte genau einen Vornamen und einen Nachnamen eingeben, getrennt durch Leerzeichen."
        );
        return;
    }

    // Extrahiert Vorname (Gruppe 1) und Nachname (Gruppe 2).
    const QString firstName = match.captured(1);
    const QString lastName = match.captured(2);

    // Speichert das Paar in namesByLastName.
    // QMap sortiert automatisch nach dem Schlüssel (Nachname).
    namesByLastName[lastName] = firstName;

    // Leert das Eingabefeld für die nächste Eingabe.
    ui->lineEdit->clear();
}
