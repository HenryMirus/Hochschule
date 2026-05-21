#ifndef PATIENTAPI_H
#define PATIENTAPI_H

#include "patient.h"

#include <QList>

// Autor: Henry Mirus
// Zweck: UI-unabhängige API zur Verwaltung der Patientenliste mit CRUD-
// Operationen und Sortierung nach Nachname, Vorname und Geburtsdatum.
class PatientApi
{
public:
    // Autor: Henry Mirus
    // Eingabe: `patient` als neuer Datensatz.
    // Rückgabe: keine.
    // Seiteneffekte: Der Patient wird in die interne Liste eingefügt und die
    // Liste wird anschließend sortiert.
    PatientApi() = default;

    // Autor: Henry Mirus
    // Eingabe: `patient` als neuer Datensatz.
    // Rückgabe: keine.
    // Seiteneffekte: Fügt den Patienten hinzu und sortiert die interne Liste.
    void create(const Patient &patient);

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: Referenz auf die komplette sortierte Liste.
    // Seiteneffekte: keine.
    const QList<Patient>& readAll() const;

    // Autor: Henry Mirus
    // Eingabe: `index` des gewünschten Patienten, `outPatient` als Ausgabepuffer.
    // Rückgabe: `true` bei gültigem Index, sonst `false`.
    // Seiteneffekte: Schreibt den gefundenen Patienten in `outPatient`.
    bool readAt(int index, Patient &outPatient) const;

    // Autor: Henry Mirus
    // Eingabe: `index` des zu ersetzenden Datensatzes und `patient` als neuer Wert.
    // Rückgabe: `true` bei Erfolg, sonst `false`.
    // Seiteneffekte: Ersetzt einen Datensatz und sortiert die Liste neu.
    bool update(int index, const Patient &patient);

    // Autor: Henry Mirus
    // Eingabe: `index` des zu löschenden Datensatzes.
    // Rückgabe: `true` bei Erfolg, sonst `false`.
    // Seiteneffekte: Entfernt einen Datensatz aus der internen Liste.
    bool remove(int index);

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: Anzahl der gespeicherten Patienten.
    // Seiteneffekte: keine.
    int size() const;

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Löscht alle Datensätze aus der internen Liste.
    void clear();

    // Autor: Henry Mirus
    // Eingabe: `patients` als komplette neue Liste.
    // Rückgabe: keine.
    // Seiteneffekte: Ersetzt die interne Liste und sortiert sie neu.
    void setAll(const QList<Patient> &patients);

private:
    QList<Patient> m_patients;

    // Autor: Henry Mirus
    // Eingabe: Zwei Patienten zur Vergleichsentscheidung.
    // Rückgabe: `true`, wenn `lhs` vor `rhs` sortiert werden soll.
    // Seiteneffekte: keine.
    static bool lessThanForList(const Patient &lhs, const Patient &rhs);

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Sortiert die interne Patientenliste neu.
    void sortPatients();
};

#endif // PATIENTAPI_H
