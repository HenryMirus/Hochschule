#ifndef PATIENTJSONSTORAGE_H
#define PATIENTJSONSTORAGE_H

#include "patient.h"

#include <QList>
#include <QString>

// Autor: Henry Mirus
// Zweck: Persistenzklasse zum Laden und Speichern der Patientenliste in eine
// feste JSON-Datei ohne Geschäftslogik.
class PatientJsonStorage
{
public:
    // Autor: Henry Mirus
    // Eingabe: `filePath` für die JSON-Datei, `outPatients` als Ausgabeliste,
    // `errorMessage` optional für eine Fehlermeldung.
    // Rückgabe: `true` bei Erfolg, sonst `false`.
    // Seiteneffekte: Füllt `outPatients` mit den geladenen Daten.
    static bool load(const QString &filePath, QList<Patient> &outPatients, QString *errorMessage = nullptr);

    // Autor: Henry Mirus
    // Eingabe: `filePath` für die JSON-Datei, `patients` als zu speichernde Liste,
    // `errorMessage` optional für eine Fehlermeldung.
    // Rückgabe: `true` bei Erfolg, sonst `false`.
    // Seiteneffekte: Schreibt die Liste in die Datei.
    static bool save(const QString &filePath, const QList<Patient> &patients, QString *errorMessage = nullptr);
};

#endif // PATIENTJSONSTORAGE_H
