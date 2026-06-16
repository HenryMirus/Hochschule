#include "einstellungsdialog.h"
#include "ui_einstellungsdialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

/*
 * Autor: Henry Mirus
 * Eingabe: parent – übergeordnetes Widget
 * Rückgabe: –
 * Seiteneffekte: Initialisiert das per Qt-Designer erzeugte UI und
 *                verbindet OK- und Abbrechen-Button mit den Slots.
 */
EinstellungsDialog::EinstellungsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EinstellungsDialog)
{
    ui->setupUi(this);
    setSizeGripEnabled(true);

    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &EinstellungsDialog::onOkGeklickt);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Gibt das UI-Objekt frei.
 */
EinstellungsDialog::~EinstellungsDialog()
{
    delete ui;
}

/*
 * Autor: Henry Mirus
 * Eingabe: e – Spieleinstellungen, die angezeigt werden sollen
 * Rückgabe: –
 * Seiteneffekte: Schreibt die Werte aus e in die Spinboxen und
 *                speichert e als Fallback in m_einstellungen.
 */
void EinstellungsDialog::setEinstellungen(const HanoiEinstellungen &e)
{
    m_einstellungen = e;
    ui->scheibenSpinBox->setValue(e.scheibenzahl);
    ui->zugdauerSpinBox->setValue(e.zugdauerMs);
    ui->farbschemaComboBox->setCurrentIndex(
        e.farbschema == Farbschema::Regenbogen ? 1 : 0);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Zuletzt per OK bestätigte Einstellungen
 * Seiteneffekte: –
 */
HanoiEinstellungen EinstellungsDialog::einstellungen() const
{
    return m_einstellungen;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Überträgt die aktuellen Spinbox-Werte in m_einstellungen
 *                und schließt den Dialog mit Accepted-Ergebnis.
 */
void EinstellungsDialog::onOkGeklickt()
{
    m_einstellungen.scheibenzahl = ui->scheibenSpinBox->value();
    m_einstellungen.zugdauerMs   = ui->zugdauerSpinBox->value();
    m_einstellungen.farbschema   = (ui->farbschemaComboBox->currentIndex() == 1)
                                   ? Farbschema::Regenbogen
                                   : Farbschema::Gold;
    accept();
}
