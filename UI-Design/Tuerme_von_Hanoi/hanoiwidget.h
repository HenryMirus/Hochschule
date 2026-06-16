#ifndef HANOIWIDGET_H
#define HANOIWIDGET_H

#include <QElapsedTimer>
#include <QWidget>
#include "hanoi.h"
#include "hanoieinstellungen.h"

class QTimer;

/*
 * Autor: Henry Mirus
 * Zweck: Zustandsautomat für den Animationsablauf:
 *        Bereit   – Startkonfiguration angezeigt, noch keine Animation
 *        Laeuft   – Animation läuft
 *        Pausiert – Animation angehalten (Statuszeile blinkt im MainWindow)
 *        Beendet  – Alle Züge ausgeführt
 */
enum class AnimationsZustand
{
    Bereit,
    Laeuft,
    Pausiert,
    Beendet
};

/*
 * Autor: Henry Mirus
 * Zweck: Benutzerdefiniertes QWidget, das die drei Pflöcke und die Scheiben
 *        grafisch darstellt und die Bewegungsanimation steuert.
 *
 *        Animationspfad: kubische Bézier-Kurve mit zwei Kontrollpunkten auf
 *        Freigabehöhe (Bogen über den Pfosten).
 *        Geschwindigkeitsfunktion: Smoothstep f(t) = t²·(3−2t), die eine
 *        sanfte Beschleunigung am Anfang und Verzögerung am Ende bewirkt.
 *        Die Zugdauer ist über HanoiEinstellungen frei einstellbar.
 */
class HanoiWidget : public QWidget
{
    Q_OBJECT

public:
    /*
     * Autor: Henry Mirus
     * Eingabe: parent – übergeordnetes Widget
     * Rückgabe: –
     * Seiteneffekte: Initialisiert das Spiel mit Standardeinstellungen (5 Scheiben,
     *                800 ms/Zug) und startet den internen Animationstimer.
     */
    explicit HanoiWidget(QWidget *parent = nullptr);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Empfohlene Fenstergröße, abhängig von der Scheibenanzahl
     * Seiteneffekte: –
     */
    QSize sizeHint() const override;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Absolute Mindestgröße des Widgets
     * Seiteneffekte: –
     */
    QSize minimumSizeHint() const override;

public slots:
    /*
     * Autor: Henry Mirus
     * Eingabe: e – neue Einstellungen (Scheibenanzahl, Zugdauer)
     * Rückgabe: –
     * Seiteneffekte: Stoppt laufende Animation, initialisiert das Spiel neu,
     *                setzt den Animationszustand auf Bereit und sendet Signale.
     */
    void neuesSpiel(const HanoiEinstellungen &e);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Startet die Animation (nur im Zustand Bereit möglich).
     *                Setzt m_t = 0, startet den QElapsedTimer und den Animationstimer.
     */
    void starteAnimation();

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Im Zustand Laeuft: pausiert (Timer stopp, Elapsed akkumuliert).
     *                Im Zustand Pausiert: setzt die Animation fort.
     */
    void pauseWechsel();

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Bricht die laufende oder pausierte Animation ab. Konstruiert
     *                den Endzustand direkt (alle Scheiben auf Pflock 2) und setzt
     *                den Zustand auf Beendet. Im Zustand Bereit oder Beendet wirkungslos.
     */
    void beendeAnimation();

signals:
    /*
     * Autor: Henry Mirus
     * Eingabe: zustand – neuer AnimationsZustand
     * Rückgabe: –
     */
    void zustandGeaendert(AnimationsZustand zustand);

    /*
     * Autor: Henry Mirus
     * Eingabe: fertigeZuege – Anzahl abgeschlossener Züge
     *          gesamtZuege  – Gesamtanzahl der Züge (2^n − 1)
     * Rückgabe: –
     */
    void zugFortschrittGeaendert(quint64 fertigeZuege, quint64 gesamtZuege);

protected:
    /*
     * Autor: Henry Mirus
     * Eingabe: event – Paint-Ereignis
     * Rückgabe: –
     * Seiteneffekte: Zeichnet Hintergrund, Basis, Pflöcke, statische Scheiben
     *                und (wenn animiert) die aktuelle Scheibe auf ihrer Bézier-Bahn.
     */
    void paintEvent(QPaintEvent *event) override;

private slots:
    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Wird ca. alle 16 ms vom m_animTimer ausgelöst. Berechnet
     *                den Animationsfortschritt t, wendet abgeschlossene Züge
     *                auf m_animPegs an, sendet zugFortschrittGeaendert und
     *                ruft update() auf.
     */
    void onAnimTimer();

private:
    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Kopiert den Anfangszustand von m_spiel (alle Scheiben auf
     *                Pflock 0) in m_animPegs[0] und leert die anderen Pflöcke.
     */
    void initialisiereAnimationszustand();

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Liest von/nach aus dem Lazy-Generator, ruft
     *                m_spiel.executeAndAdvance() auf und überträgt den Zug auf
     *                m_animPegs.
     */
    void applyCurrentMove();

    // --- Spiellogik ---
    HanoiSpiel       m_spiel;
    Pflock           m_animPegs[3];  // Laufender Animationszustand der drei Pflöcke

    // --- Animationssteuerung ---
    qreal            m_t = 0.0;                 // Fortschritt des aktuellen Zugs [0.0, 1.0]
    qint64           m_zugElapsedBeforeMs = 0;  // Akkumulierte Zeit vor einer Pause
    QElapsedTimer    m_zugTimer;                 // Misst Elapsed seit Zugbeginn
    QTimer          *m_animTimer = nullptr;      // Feuert mit ca. 60 FPS

    // --- Zustand & Einstellungen ---
    AnimationsZustand  m_zustand = AnimationsZustand::Bereit;
    HanoiEinstellungen m_einstellungen;
};

#endif // HANOIWIDGET_H
