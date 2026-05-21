#ifndef PATIENTTABLEMODEL_H
#define PATIENTTABLEMODEL_H

#include <QAbstractTableModel>

#include "patient.h"

// Autor: Henry Mirus
// Zweck: Qt-TableModel zur Anzeige der Patientenliste in der Hauptansicht mit
// den Spalten Name, Vorname und Geburtsdatum.
class PatientTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    // Autor: Henry Mirus
    // Eingabe: optionales QObject-Elternobjekt.
    // Rückgabe: keine.
    // Seiteneffekte: Initialisiert das TableModel.
    explicit PatientTableModel(QObject *parent = nullptr);

    // Autor: Henry Mirus
    // Eingabe: `patients` als neue Anzeigeliste.
    // Rückgabe: keine.
    // Seiteneffekte: Ersetzt die aktuelle Modell-Datenbasis.
    void setPatients(const QList<Patient> &patients);

    // Autor: Henry Mirus
    // Eingabe: `row` als Zeilenindex.
    // Rückgabe: Referenz auf den Patienten der Zeile.
    // Seiteneffekte: keine.
    const Patient& patientAt(int row) const;

    // Autor: Henry Mirus
    // Eingabe: optionaler Elternindex.
    // Rückgabe: Anzahl der Zeilen.
    // Seiteneffekte: keine.
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Autor: Henry Mirus
    // Eingabe: optionaler Elternindex.
    // Rückgabe: Anzahl der Spalten.
    // Seiteneffekte: keine.
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Autor: Henry Mirus
    // Eingabe: `index` und angeforderte `role`.
    // Rückgabe: Anzeigenwert für die Zelle.
    // Seiteneffekte: keine.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Autor: Henry Mirus
    // Eingabe: Spaltenindex, Orientierung und `role`.
    // Rückgabe: Kopfzeilentext oder ein leerer Wert.
    // Seiteneffekte: keine.
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QList<Patient> m_patients;
};

#endif // PATIENTTABLEMODEL_H
