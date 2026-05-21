#ifndef PATIENT_H
#define PATIENT_H

#include <QDate>
#include <QString>

// Autor: Henry Mirus
// Zweck: Domänenobjekt für einen Patienten. Diese Klasse enthält ausschließlich
// die fachlichen Datenfelder ohne UI- oder Persistenzlogik.
class Patient
{
public:
    QString title;
    QString lastName;
    QString firstName;
    QString street;
    QString city;
    QString phone;
    QDate birthday;
    QString gender;
};

#endif // PATIENT_H
