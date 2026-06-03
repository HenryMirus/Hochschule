#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SettingsDialog;
}
QT_END_NAMESPACE

/*
 * Autor: Henry Mirus
 * Zweck: Dialog zur Konfiguration der Spielparameter (Zeilen n, Spalten m,
 *        Minenanzahl k). Bietet einen Bestätigen- und einen Abbrechen-Schalter.
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

    void setSettings(const GameSettings &settings);
    GameSettings settings() const;

private slots:
    void applyCurrentSettings();
    void updateMineConstraints();

private:
    Ui::SettingsDialog *ui;
    GameSettings m_settings;
};

#endif // SETTINGSDIALOG_H
