#ifndef EINSTELLUNGSDIALOG_H
#define EINSTELLUNGSDIALOG_H

#include <QDialog>
#include "hanoieinstellungen.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EinstellungsDialog; }
QT_END_NAMESPACE

/*
 * Autor: Henry Mirus
 * Zweck: Modaler Dialog zur Konfiguration der Spielparameter.
 *        Ermöglicht das Ändern der Scheibenanzahl (1–12) und der Zugdauer
 *        (100–5000 ms). Wird per Qt-Designer entworfen.
 */
class EinstellungsDialog : public QDialog
{
    Q_OBJECT

public:
    /*
     * Autor: Henry Mirus
     * Eingabe: parent – übergeordnetes Widget
     * Rückgabe: –
     * Seiteneffekte: Verbindet OK-/Abbrechen-Button mit den zugehörigen Slots.
     */
    explicit EinstellungsDialog(QWidget *parent = nullptr);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Gibt das UI-Objekt frei.
     */
    ~EinstellungsDialog() override;

    /*
     * Autor: Henry Mirus
     * Eingabe: e – Einstellungen, die in die Spinboxen übernommen werden
     * Rückgabe: –
     * Seiteneffekte: Setzt die Spinboxwerte und speichert e in m_einstellungen.
     */
    void setEinstellungen(const HanoiEinstellungen &e);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Zuletzt bestätigte Einstellungen
     * Seiteneffekte: –
     */
    HanoiEinstellungen einstellungen() const;

private slots:
    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Liest Spinboxwerte aus, schreibt sie in m_einstellungen
     *                und schließt den Dialog mit accept().
     */
    void onOkGeklickt();

private:
    Ui::EinstellungsDialog *ui;
    HanoiEinstellungen      m_einstellungen;
};

#endif // EINSTELLUNGSDIALOG_H
