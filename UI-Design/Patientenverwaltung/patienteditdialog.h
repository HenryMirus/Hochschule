#ifndef PATIENTEDITDIALOG_H
#define PATIENTEDITDIALOG_H

#include <QDialog>
#include "patient.h"

namespace Ui {
class patienteditdialog;
}

// Autor: Henry Mirus
// Zweck: Dialog zum Erstellen und Bearbeiten vollständiger Patientendaten.
class patienteditdialog : public QDialog
{
    Q_OBJECT

public:
    // Autor: Henry Mirus
    // Eingabe: optionales Elternobjekt.
    // Rückgabe: keine.
    // Seiteneffekte: Initialisiert den Bearbeitungsdialog.
    explicit patienteditdialog(QWidget *parent = nullptr);

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Gibt interne Qt-Ressourcen frei.
    ~patienteditdialog();

    // Autor: Henry Mirus
    // Eingabe: `p` als bestehender Patientendatensatz.
    // Rückgabe: keine.
    // Seiteneffekte: Füllt die Eingabefelder für die Bearbeitung.
    void setPatient(const Patient &p);

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: Patient mit den aktuellen Dialogwerten.
    // Seiteneffekte: keine.
    Patient patient() const;

public slots:
    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Prüft Pflichtfelder und schließt den Dialog nur bei Erfolg.
    void accept() override;

private:
    // Autor: Henry Mirus
    // Eingabe: optionaler Speicher für eine Fehlermeldung.
    // Rückgabe: `true`, wenn alle Pflichtfelder vorhanden und gültig sind.
    // Seiteneffekte: keine.
    bool hasRequiredFields(QString *errorMessage = nullptr) const;

    Ui::patienteditdialog *ui;
};

#endif // PATIENTEDITDIALOG_H
