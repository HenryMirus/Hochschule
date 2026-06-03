#include "statisticdialog.h"
#include "ui_statisticdialog.h"

#include "statistics.h"

#include <QDialogButtonBox>
#include <QHeaderView>
#include <QTableWidgetItem>

/*
 * Autor: Henry Mirus
 * Eingabe: parent – übergeordnetes Widget
 * Rückgabe: –
 * Seiteneffekte: Lädt die gespeicherten Statistiken und richtet die
 *                Tabellenspalten auf Strecken ein.
 */
StatisticDialog::StatisticDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatisticDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    reloadStatistics();

    ui->statisticsTable->horizontalHeader()->setStretchLastSection(true);
    ui->statisticsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Gibt das UI-Objekt frei.
 */
StatisticDialog::~StatisticDialog()
{
    delete ui;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Liest die Statistikdatei neu ein und befüllt die Tabelle
 *                mit einem Eintrag pro Spielkonfiguration.
 */
void StatisticDialog::reloadStatistics()
{
    const QVector<StatisticEntry> entries = Statistics::loadStatistics();
    ui->statisticsTable->setRowCount(entries.size());

    for (int row = 0; row < entries.size(); ++row) {
        const StatisticEntry &entry = entries.at(row);
        ui->statisticsTable->setItem(row, 0, new QTableWidgetItem(QStringLiteral("%1 x %2").arg(entry.settings.rows).arg(entry.settings.columns)));
        ui->statisticsTable->setItem(row, 1, new QTableWidgetItem(QString::number(entry.settings.mines)));
        ui->statisticsTable->setItem(row, 2, new QTableWidgetItem(QString::number(entry.gamesPlayed)));
        ui->statisticsTable->setItem(row, 3, new QTableWidgetItem(QString::number(entry.wins)));
        ui->statisticsTable->setItem(row, 4, new QTableWidgetItem(QString::number(entry.losses)));
        ui->statisticsTable->setItem(row, 5, new QTableWidgetItem(entry.bestWinTimeSeconds >= 0 ? QStringLiteral("%1 s").arg(entry.bestWinTimeSeconds) : QStringLiteral("n/a")));
    }
}
