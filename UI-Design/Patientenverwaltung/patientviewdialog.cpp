#include "patientviewdialog.h"
#include "ui_patientviewdialog.h"

patientviewdialog::patientviewdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::patientviewdialog)
{
    ui->setupUi(this);
}

patientviewdialog::~patientviewdialog()
{
    delete ui;
}

void patientviewdialog::setPatient(const Patient &p)
{
    ui->TitleLineEdit->setText(p.title);
    ui->NameLineEdit->setText(p.lastName);
    ui->FirstNameLineEdit->setText(p.firstName);
    ui->StreetLineEdit->setText(p.street);
    ui->CityLineEdit->setText(p.city);
    ui->PhoneNrLineEdit->setText(p.phone);
    ui->GenderLineEdit->setText(p.gender);
    ui->BirthdayLineEdit->setText(p.birthday.isValid() ? p.birthday.toString("dd.MM.yyyy") : QString());
}

