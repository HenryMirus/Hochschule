#ifndef STATISTICDIALOG_H
#define STATISTICDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class StatisticDialog;
}
QT_END_NAMESPACE

/*
 * Autor: Henry Mirus
 * Zweck: Dialog zur Anzeige der gespeicherten Spielstatistiken, gruppiert
 *        nach Spielkonfiguration (Zeilen, Spalten, Minenanzahl).
 */
class StatisticDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticDialog(QWidget *parent = nullptr);
    ~StatisticDialog() override;

    void reloadStatistics();

private:
    Ui::StatisticDialog *ui;
};

#endif // STATISTICDIALOG_H
