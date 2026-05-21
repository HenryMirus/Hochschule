#include "patienteditdialog.h"
#include "ui_patienteditdialog.h"

#include <QDate>

patienteditdialog::patienteditdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::patienteditdialog)
{
    ui->setupUi(this);
    // Der fachlich relevante Default-Button ist "Übernehmen".
    // Dadurch wird er beim Öffnen des Dialogs blau umrandet und per Enter ausgelöst.
    ui->acceptButton->setDefault(true);
    ui->acceptButton->setAutoDefault(true);
    ui->rejectButton->setDefault(false);
    ui->rejectButton->setAutoDefault(false);
    ui->acceptButton->setFocus();
}

patienteditdialog::~patienteditdialog()
{
    delete ui;
}

bool patienteditdialog::hasRequiredFields(QString *errorMessage) const
{
    const QString lastName = ui->NameLineEdit->text().trimmed();
    const QString firstName = ui->FirstNameLineEdit->text().trimmed();
    const QString birthdayText = ui->BirthdayLineEdit->text().trimmed();
    const QDate birthday = QDate::fromString(birthdayText, "dd.MM.yyyy");

    if (lastName.isEmpty()) {
        if (errorMessage) {
            *errorMessage = "Der Name ist ein Pflichtfeld.";
        }
        return false;
    }

    if (firstName.isEmpty()) {
        if (errorMessage) {
            *errorMessage = "Der Vorname ist ein Pflichtfeld.";
        }
        return false;
    }

    if (birthdayText.isEmpty()) {
        if (errorMessage) {
            *errorMessage = "Das Geburtsdatum ist ein Pflichtfeld.";
        }
        return false;
    }

    if (!birthday.isValid()) {
        if (errorMessage) {
            *errorMessage = "Bitte ein gültiges Geburtsdatum im Format TT.MM.JJJJ eingeben.";
        }
        return false;
    }

    return true;
}

void patienteditdialog::setPatient(const Patient &p)
{
    ui->TitleLineEdit->setText(p.title);
    ui->NameLineEdit->setText(p.lastName);
    ui->FirstNameLineEdit->setText(p.firstName);
    ui->StreetLineEdit->setText(p.street);
    ui->CityLineEdit->setText(p.city);
    ui->PhoneNrLineEdit->setText(p.phone);
    ui->GenderLineEdit->setText(p.gender);
    if (p.birthday.isValid()) {
        ui->BirthdayLineEdit->setText(p.birthday.toString("dd.MM.yyyy"));
    } else {
        ui->BirthdayLineEdit->clear();
    }
}

Patient patienteditdialog::patient() const
{
    Patient p;
    p.title = ui->TitleLineEdit->text();
    p.lastName = ui->NameLineEdit->text();
    p.firstName = ui->FirstNameLineEdit->text();
    p.street = ui->StreetLineEdit->text();
    p.city = ui->CityLineEdit->text();
    p.phone = ui->PhoneNrLineEdit->text();
    p.gender = ui->GenderLineEdit->text();
    const QString b = ui->BirthdayLineEdit->text();
    // Erwartetes Eingabeformat: DD.MM.YYYY
    p.birthday = QDate::fromString(b, "dd.MM.yyyy");
    return p;
}

void patienteditdialog::accept()
{
    QString errorMessage;
    if (!hasRequiredFields(&errorMessage)) {
        ui->ErrorMessageLabel->setText(errorMessage);
        if (ui->NameLineEdit->text().trimmed().isEmpty()) {
            ui->NameLineEdit->setFocus();
        } else if (ui->FirstNameLineEdit->text().trimmed().isEmpty()) {
            ui->FirstNameLineEdit->setFocus();
        } else {
            ui->BirthdayLineEdit->setFocus();
        }
        return;
    }

    ui->ErrorMessageLabel->clear();
    QDialog::accept();
}
