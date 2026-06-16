#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "hanoieinstellungen.h"
#include "hanoiwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;
class QTimer;
class EinstellungsDialog;

/*
 * Autor: Henry Mirus
 * Zweck: Hauptfenster der Türme-von-Hanoi-Anwendung.
 *        Zeigt das HanoiWidget mit der Animation, steuert über vier Schalter
 *        (Neu, Start, Pause, Beenden) den Animationsablauf, aktualisiert die
 *        Statuszeile (Scheibenzahl, Zugfortschritt, Animationszustand) und
 *        öffnet bei Bedarf den Einstellungsdialog.
 *
 *        Während einer Pause blinkt die Zustandsanzeige in der Statuszeile
 *        durch einen eigenen Blink-Timer (500 ms Takt).
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*
     * Autor: Henry Mirus
     * Eingabe: parent – übergeordnetes Widget
     * Rückgabe: –
     * Seiteneffekte: Baut UI auf, erzeugt Statusleisten-Labels, verbindet alle
     *                Signale/Slots und startet mit den Standardeinstellungen.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Gibt UI-Objekt frei.
     */
    ~MainWindow() override;

private slots:
    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Startet ein neues Spiel mit m_einstellungen: Setzt lokale
     *                Zähler zurück und ruft hanoiWidget->neuesSpiel() auf.
     */
    void onNeuGeklickt();

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Öffnet EinstellungsDialog; bei Bestätigung werden die
     *                neuen Einstellungen übernommen und ein neues Spiel gestartet.
     */
    void openEinstellungsDialog();

    /*
     * Autor: Henry Mirus
     * Eingabe: zustand – neuer AnimationsZustand vom HanoiWidget
     * Rückgabe: –
     * Seiteneffekte: Passt die Schaltflächen-Texte/-Aktivierung an und
     *                startet oder stoppt den Blink-Timer.
     */
    void onZustandGeaendert(AnimationsZustand zustand);

    /*
     * Autor: Henry Mirus
     * Eingabe: fertigeZuege – Anzahl abgeschlossener Züge
     *          gesamtZuege  – Gesamtanzahl der Züge
     * Rückgabe: –
     * Seiteneffekte: Aktualisiert m_aktuellZugIndex und ruft updateStatusBar() auf.
     */
    void onZugFortschrittGeaendert(quint64 fertigeZuege, quint64 gesamtZuege);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Schaltet m_blinkSichtbar um und aktualisiert den Zustandslabel.
     */
    void onBlinkTimer();

private:
    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Schreibt Scheibenanzahl, Zugfortschritt und Zustandstext
     *                in die drei Statusleisten-Labels.
     */
    void updateStatusBar();

    Ui::MainWindow     *ui;
    EinstellungsDialog *m_einstellungsDialog = nullptr;
    HanoiEinstellungen  m_einstellungen;

    QLabel *m_scheibenLabel = nullptr;  // "Scheiben: N"
    QLabel *m_zugLabel      = nullptr;  // "Zug: X / Y"
    QLabel *m_zustandLabel  = nullptr;  // Zustandstext (blinkt bei Pause)

    QTimer *m_blinkTimer    = nullptr;
    bool    m_blinkSichtbar = true;

    AnimationsZustand m_aktuellZustand  = AnimationsZustand::Bereit;
    quint64           m_aktuellZugIndex = 0;
    quint64           m_gesamtZuege     = 0;
};

#endif // MAINWINDOW_H
