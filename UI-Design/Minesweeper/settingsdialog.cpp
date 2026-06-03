#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDialogButtonBox>
#include <QSpinBox>

/*
 * Autor: Henry Mirus
 * Eingabe: parent – übergeordnetes Widget
 * Rückgabe: –
 * Seiteneffekte: Verbindet Schaltflächen und Spinboxen mit den zugehörigen Slots.
 */
SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setSizeGripEnabled(true);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::applyCurrentSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->rowsSpinBox,    qOverload<int>(&QSpinBox::valueChanged), this, &SettingsDialog::updateMineConstraints);
    connect(ui->columnsSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &SettingsDialog::updateMineConstraints);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Gibt das UI-Objekt frei.
 */
SettingsDialog::~SettingsDialog()
{
    delete ui;
}

/*
 * Autor: Henry Mirus
 * Eingabe: settings – Spielkonfiguration, die in den Dialog übernommen wird
 * Rückgabe: –
 * Seiteneffekte: Setzt die Spinboxen auf die Werte der übergebenen Konfiguration
 *                und aktualisiert die Minenanzahl-Beschränkung.
 */
void SettingsDialog::setSettings(const GameSettings &settings)
{
    m_settings = settings;
    ui->rowsSpinBox->setValue(settings.rows);
    ui->columnsSpinBox->setValue(settings.columns);
    updateMineConstraints();
    ui->minesSpinBox->setValue(settings.mines);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Zuletzt bestätigte Spielkonfiguration
 * Seiteneffekte: –
 */
GameSettings SettingsDialog::settings() const
{
    return m_settings;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Passt das Maximum der Minen-Spinbox dynamisch an die aktuell
 *                eingestellte Feldgröße an (maximal 30 % der Felder).
 */
void SettingsDialog::updateMineConstraints()
{
    const int maxMines = ui->rowsSpinBox->value() * ui->columnsSpinBox->value() * 3 / 10;
    ui->minesLabel->setText(QStringLiteral("Minen (Max. %1)").arg(maxMines));
    ui->minesSpinBox->setMaximum(maxMines);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Liest die Spinboxwerte aus, schreibt sie in m_settings,
 *                speichert die Einstellungen als JSON und schließt den Dialog
 *                mit Accept. Bei ungültiger Konfiguration wird der Dialog
 *                nicht geschlossen.
 */
void SettingsDialog::applyCurrentSettings()
{
    m_settings.rows = ui->rowsSpinBox->value();
    m_settings.columns = ui->columnsSpinBox->value();
    m_settings.mines = ui->minesSpinBox->value();

    if (!m_settings.isValid()) {
        return;
    }

    Settings::saveSettings(m_settings);
    accept();
}
