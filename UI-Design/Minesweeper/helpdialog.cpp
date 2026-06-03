#include "helpdialog.h"
#include "ui_helpdialog.h"

#include <QDialogButtonBox>

/*
 * Autor: Henry Mirus
 * Eingabe: parent – übergeordnetes Widget
 * Rückgabe: –
 * Seiteneffekte: Verbindet den Schließen-Schalter mit dem Reject-Slot.
 */
HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Gibt das UI-Objekt frei.
 */
HelpDialog::~HelpDialog()
{
    delete ui;
}
