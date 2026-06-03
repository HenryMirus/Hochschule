#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class HelpDialog;
}
QT_END_NAMESPACE

/*
 * Autor: Henry Mirus
 * Zweck: Dialog zur Anzeige der Spielanleitung. Erklärt dem Spieler,
 *        welche Aktionen durch die Maustasten ausgeführt werden.
 */
class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog() override;

private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
