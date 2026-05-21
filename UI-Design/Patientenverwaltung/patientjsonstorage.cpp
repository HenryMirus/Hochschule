#include "patientjsonstorage.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {

QJsonObject toJson(const Patient &patient)
{
    QJsonObject obj;
    obj["title"] = patient.title;
    obj["lastName"] = patient.lastName;
    obj["firstName"] = patient.firstName;
    obj["street"] = patient.street;
    obj["city"] = patient.city;
    obj["phone"] = patient.phone;
    obj["gender"] = patient.gender;
    obj["birthday"] = patient.birthday.isValid() ? patient.birthday.toString(Qt::ISODate) : QString();
    return obj;
}

Patient fromJson(const QJsonObject &obj)
{
    Patient patient;
    patient.title = obj.value("title").toString();
    patient.lastName = obj.value("lastName").toString();
    patient.firstName = obj.value("firstName").toString();
    patient.street = obj.value("street").toString();
    patient.city = obj.value("city").toString();
    patient.phone = obj.value("phone").toString();
    patient.gender = obj.value("gender").toString();
    patient.birthday = QDate::fromString(obj.value("birthday").toString(), Qt::ISODate);
    return patient;
}

} // namespace

bool PatientJsonStorage::load(const QString &filePath, QList<Patient> &outPatients, QString *errorMessage)
{
    QFile file(filePath);
    if (!file.exists()) {
        outPatients.clear();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    const QByteArray raw = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(raw, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
        if (errorMessage) {
            *errorMessage = parseError.errorString();
        }
        return false;
    }

    QList<Patient> result;
    const QJsonArray array = doc.array();
    result.reserve(array.size());
    for (const QJsonValue &value : array) {
        if (!value.isObject()) {
            continue;
        }
        result.append(fromJson(value.toObject()));
    }

    outPatients = result;
    return true;
}

bool PatientJsonStorage::save(const QString &filePath, const QList<Patient> &patients, QString *errorMessage)
{
    QJsonArray array;
    for (const Patient &patient : patients) {
        array.append(toJson(patient));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    const QJsonDocument doc(array);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
