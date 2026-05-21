#include "patienttablemodel.h"

PatientTableModel::PatientTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void PatientTableModel::setPatients(const QList<Patient> &patients)
{
    beginResetModel();
    m_patients = patients;
    endResetModel();
}

const Patient& PatientTableModel::patientAt(int row) const
{
    return m_patients.at(row);
}

int PatientTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_patients.size();
}

int PatientTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 3;
}

QVariant PatientTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    const Patient &patient = m_patients.at(index.row());
    switch (index.column()) {
    case 0:
        return patient.lastName;
    case 1:
        return patient.firstName;
    case 2:
        return patient.birthday.isValid() ? patient.birthday.toString("dd.MM.yyyy") : QString();
    default:
        return QVariant();
    }
}

QVariant PatientTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return QStringLiteral("Name");
    case 1:
        return QStringLiteral("Vorname");
    case 2:
        return QStringLiteral("Geburtsdatum");
    default:
        return QVariant();
    }
}
