#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QCoreApplication>
#include <QBoxLayout>
#include <QHeaderView>
#include <QRegularExpression>
#include <QSizePolicy>
#include <QMessageBox>

namespace {

// Autor: Henry Mirus
// Eingabe: Suchtext aus dem Eingabefeld.
// Rückgabe: Einzelne Suchbegriffe, getrennt durch Leerzeichen oder Kommas.
// Seiteneffekte: keine.
QStringList splitSearchTerms(const QString &searchText)
{
    return searchText.trimmed().split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
}

// Autor: Henry Mirus
// Eingabe: Ein Suchbegriff und ein zu prüfendes Textfeld.
// Rückgabe: `true`, wenn beide Werte exakt übereinstimmen.
// Seiteneffekte: keine.
bool tokenMatchesField(const QString &token, const QString &field)
{
    return QString::compare(token.trimmed(), field.trimmed(), Qt::CaseInsensitive) == 0;
}

// Autor: Henry Mirus
// Eingabe: Ein Suchbegriff und ein Geburtsdatum.
// Rückgabe: `true`, wenn der Suchbegriff dem Datum im Format TT.MM.JJJJ exakt entspricht.
// Seiteneffekte: keine.
bool tokenMatchesBirthday(const QString &token, const QDate &birthday)
{
    if (!birthday.isValid()) {
        return false;
    }

    const QDate parsedDate = QDate::fromString(token.trimmed(), "dd.MM.yyyy");
    return parsedDate.isValid() && parsedDate == birthday;
}

bool matchesTermsRecursively(const QStringList &terms,
                             int termIndex,
                             const QStringList &availableFields,
                             const QDate &birthday);

// Autor: Henry Mirus
// Eingabe: Patientenobjekt und Suchtext.
// Rückgabe: `true`, wenn alle Suchparameter exakt zu Name, Vorname und/oder
// Geburtsdatum des Patienten passen.
// Seiteneffekte: keine.
bool matchesSearch(const Patient &patient, const QString &searchText)
{
    const QStringList terms = splitSearchTerms(searchText);
    if (terms.isEmpty()) {
        return true;
    }

    if (terms.size() > 3) {
        return false;
    }

    QStringList availableFields;
    availableFields << patient.lastName << patient.firstName;
    if (patient.birthday.isValid()) {
        availableFields << QStringLiteral("__birthday__");
    }

    if (terms.size() == 1) {
        const QString &term = terms.first();
        return tokenMatchesField(term, patient.lastName)
            || tokenMatchesField(term, patient.firstName)
            || tokenMatchesBirthday(term, patient.birthday);
    }

    // Reihenfolge spielt keine Rolle: jeder Suchbegriff muss genau einem Feld zugeordnet werden können.
    return matchesTermsRecursively(terms, 0, availableFields, patient.birthday);
}

// Autor: Henry Mirus
// Eingabe: Suchtext.
// Rückgabe: Anzahl der getrennten Suchbegriffe.
// Seiteneffekte: keine.
int termCount(const QString &searchText)
{
    return splitSearchTerms(searchText).size();
}

// Autor: Henry Mirus
// Eingabe: Suchbegriffe, aktueller Index, verfügbare Felder und Geburtsdatum.
// Rückgabe: `true`, wenn alle Suchbegriffe eindeutig zugeordnet werden können.
// Seiteneffekte: keine.
bool matchesTermsRecursively(const QStringList &terms,
                             int termIndex,
                             const QStringList &availableFields,
                             const QDate &birthday)
{
    if (termIndex >= terms.size()) {
        return true;
    }

    const QString &term = terms.at(termIndex);

    for (int fieldIndex = 0; fieldIndex < availableFields.size(); ++fieldIndex) {
        const QString &field = availableFields.at(fieldIndex);
        if (!field.isEmpty() && tokenMatchesField(term, field)) {
            QStringList remainingFields = availableFields;
            remainingFields.removeAt(fieldIndex);
            if (matchesTermsRecursively(terms, termIndex + 1, remainingFields, birthday)) {
                return true;
            }
        }
    }

    if (tokenMatchesBirthday(term, birthday)) {
        QStringList remainingFields = availableFields;
        const int birthdayIndex = remainingFields.indexOf(QStringLiteral("__birthday__"));
        if (birthdayIndex >= 0) {
            remainingFields.removeAt(birthdayIndex);
            if (matchesTermsRecursively(terms, termIndex + 1, remainingFields, birthday)) {
                return true;
            }
        }
    }

    return false;
}

} // namespace

bool MainWindow::hasSelectedPatientRow() const
{
    if (ui->PatientTableView->selectionModel() == nullptr) {
        return false;
    }

    return !ui->PatientTableView->selectionModel()->selectedRows().isEmpty();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Die JSON-Datei wird im Verzeichnis der ausführbaren Datei abgelegt.
    m_defaultDataFile = QCoreApplication::applicationDirPath() + "/patients.json";

    // Das Fenster soll eine feste Größe haben und nicht vom Benutzer verändert werden.
    // Den Wert kannst du hier anpassen.
    setFixedSize(900, 650);

    m_model = new PatientTableModel(this);
    ui->PatientTableView->setModel(m_model);

    // Die Tabelle soll den verfügbaren Platz im festen Fenster komplett nutzen.
    ui->PatientTableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->PatientTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->PatientTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    if (QBoxLayout *layout = qobject_cast<QBoxLayout*>(ui->centralwidget->findChild<QObject*>("verticalLayout_2"))) {
        layout->setStretch(0, 0);
        layout->setStretch(1, 1);
        layout->setStretch(2, 0);
    }

    // Beim Programmstart automatisch Datei laden.
    QList<Patient> loadedPatients;
    QString errorMessage;
    if (!PatientJsonStorage::load(m_defaultDataFile, loadedPatients, &errorMessage)) {
        QMessageBox::warning(this,
            "Ladefehler",
            QString("Patientendatei konnte nicht gelesen werden: %1").arg(errorMessage));
    } else {
        m_patientApi.setAll(loadedPatients);
    }

    reloadTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ViewButton_clicked()
{
    if (!hasSelectedPatientRow()) {
        QMessageBox::information(this, "Hinweis", "Bitte wählen Sie zuerst einen Patienten aus.");
        return;
    }

    const int sourceIndex = selectedSourceIndex();
    if (sourceIndex < 0) {
        return;
    }

    Patient patient;
    if (!m_patientApi.readAt(sourceIndex, patient)) {
        return;
    }

    patientviewdialog dlg(this);
    dlg.setPatient(patient);
    dlg.exec();
}

void MainWindow::on_EditButton_clicked()
{
    if (!hasSelectedPatientRow()) {
        QMessageBox::information(this, "Hinweis", "Bitte wählen Sie zuerst einen Patienten aus.");
        return;
    }

    const int sourceIndex = selectedSourceIndex();
    if (sourceIndex < 0) {
        return;
    }

    Patient current;
    if (!m_patientApi.readAt(sourceIndex, current)) {
        return;
    }

    patienteditdialog dlg(this);
    dlg.setPatient(current);
    if (dlg.exec() == QDialog::Accepted) {
        const Patient updated = dlg.patient();
        m_patientApi.update(sourceIndex, updated);
        reloadTable();
    }
}

void MainWindow::on_DeleteButton_clicked()
{
    if (!hasSelectedPatientRow()) {
        QMessageBox::information(this, "Hinweis", "Bitte wählen Sie zuerst einen Patienten aus.");
        return;
    }

    const QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Löschen bestätigen",
        "Möchten Sie den ausgewählten Patienten wirklich löschen?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    const int sourceIndex = selectedSourceIndex();
    if (sourceIndex < 0) {
        return;
    }

    m_patientApi.remove(sourceIndex);
    reloadTable();
    QMessageBox::information(this, "Löschen", "Der ausgewählte Patient wurde gelöscht.");
}

void MainWindow::on_AddButton_clicked()
{
    patienteditdialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        const Patient patient = dlg.patient();
        m_patientApi.create(patient);
        reloadTable();
    }
}

void MainWindow::on_SearchButton_clicked()
{
    if (termCount(ui->SearchLineEdit->text()) > 3) {
        QMessageBox::information(this,
                                 "Hinweis",
                                 "Es sind maximal drei Suchparameter erlaubt (Name, Vorname, Geburtsdatum).\n"
                                 "Trennen Sie die Parameter mit Leerzeichen oder Komma.");
        return;
    }

    reloadTable();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QString errorMessage;
    if (!PatientJsonStorage::save(m_defaultDataFile, m_patientApi.readAll(), &errorMessage)) {
        QMessageBox::warning(this,
                             "Speicherfehler",
                             QString("Patientendatei konnte nicht gespeichert werden: %1").arg(errorMessage));
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::on_ClearSelectionButton_clicked()
{
    if (ui->PatientTableView->selectionModel() != nullptr) {
        ui->PatientTableView->selectionModel()->clearSelection();
    }
}

int MainWindow::selectedSourceIndex() const
{
    if (ui->PatientTableView->selectionModel() == nullptr) {
        return -1;
    }

    const QModelIndexList rows = ui->PatientTableView->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        return -1;
    }

    const int row = rows.first().row();
    if (row < 0 || row >= m_rowToSourceIndex.size()) {
        return -1;
    }

    return m_rowToSourceIndex.at(row);
}

void MainWindow::reloadTable()
{
    QList<Patient> visible;
    m_rowToSourceIndex.clear();

    const QString searchText = ui->SearchLineEdit->text();
    const QList<Patient> &allPatients = m_patientApi.readAll();

    for (int i = 0; i < allPatients.size(); ++i) {
        const Patient &patient = allPatients.at(i);
        if (matchesSearch(patient, searchText)) {
            visible.append(patient);
            m_rowToSourceIndex.append(i);
        }
    }

    m_model->setPatients(visible);
}
