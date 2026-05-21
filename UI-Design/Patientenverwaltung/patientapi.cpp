#include "patientapi.h"

#include <algorithm>

void PatientApi::create(const Patient &patient)
{
    m_patients.append(patient);
    sortPatients();
}

const QList<Patient>& PatientApi::readAll() const
{
    return m_patients;
}

bool PatientApi::readAt(int index, Patient &outPatient) const
{
    if (index < 0 || index >= m_patients.size()) {
        return false;
    }

    outPatient = m_patients.at(index);
    return true;
}

bool PatientApi::update(int index, const Patient &patient)
{
    if (index < 0 || index >= m_patients.size()) {
        return false;
    }

    m_patients[index] = patient;
    sortPatients();
    return true;
}

bool PatientApi::remove(int index)
{
    if (index < 0 || index >= m_patients.size()) {
        return false;
    }

    m_patients.removeAt(index);
    return true;
}

int PatientApi::size() const
{
    return m_patients.size();
}

void PatientApi::clear()
{
    m_patients.clear();
}

void PatientApi::setAll(const QList<Patient> &patients)
{
    m_patients = patients;
    sortPatients();
}


bool PatientApi::lessThanForList(const Patient &lhs, const Patient &rhs)
{
    const int byLastName = QString::compare(lhs.lastName, rhs.lastName, Qt::CaseInsensitive);
    if (byLastName != 0) {
        return byLastName < 0;
    }

    const int byFirstName = QString::compare(lhs.firstName, rhs.firstName, Qt::CaseInsensitive);
    if (byFirstName != 0) {
        return byFirstName < 0;
    }

    // Ungültige Geburtsdaten werden am Ende eingeordnet.
    if (lhs.birthday.isValid() && rhs.birthday.isValid()) {
        return lhs.birthday < rhs.birthday;
    }
    if (lhs.birthday.isValid() && !rhs.birthday.isValid()) {
        return true;
    }
    if (!lhs.birthday.isValid() && rhs.birthday.isValid()) {
        return false;
    }
    return false;
}

void PatientApi::sortPatients()
{
    std::sort(m_patients.begin(), m_patients.end(), lessThanForList);
}
