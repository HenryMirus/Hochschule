#ifndef PATIENTVIEWDIALOG_H
#define PATIENTVIEWDIALOG_H

#include <QDialog>
#include "patient.h"

namespace Ui {
class patientviewdialog;
}

// Autor: Henry Mirus
// Zweck: Dialog zur reinen Anzeige der vollständigen Patienteninformationen.
class patientviewdialog : public QDialog
{
    Q_OBJECT

public:
    // Autor: Henry Mirus
    // Eingabe: optionales Elternobjekt.
    // Rückgabe: keine.
    // Seiteneffekte: Initialisiert den Anzeigedialog.
    explicit patientviewdialog(QWidget *parent = nullptr);

    // Autor: Henry Mirus
    // Eingabe: keine.
    // Rückgabe: keine.
    // Seiteneffekte: Gibt interne Qt-Ressourcen frei.
    ~patientviewdialog();

    // Autor: Henry Mirus
    // Eingabe: `p` als Patientendaten für die Anzeige.
    // Rückgabe: keine.
    // Seiteneffekte: Befüllt die Dialogfelder mit den Patientendaten.
    void setPatient(const Patient &p);

private:
    Ui::patientviewdialog *ui;
};

#endif // PATIENTVIEWDIALOG_H
