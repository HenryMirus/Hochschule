#ifndef HANOI_H
#define HANOI_H

#include <QObject>
#include <QVector>
#include "pflock.h"

/*
 * Autor: Henry Mirus
 * Zweck: Datenstruktur für einen einzelnen Zug: Quell- und Zielpflock-Index.
 */
struct Zug
{
    int von;  // Quellpflock  (0 = links, 1 = Mitte, 2 = rechts)
    int nach; // Zielpflock
};

/*
 * Autor: Henry Mirus
 * Zweck: Implementiert die Spiellogik der Türme von Hanoi.
 *
 *        Kernidee: Statt alle 2^n − 1 Züge vorab zu speichern (bei n = 64
 *        wäre das mehr Speicher als der RAM der Erde fasst), verwendet die
 *        Klasse einen Lazy-Generator mit explizitem Rekursionsstack.
 *        Der Stack hat maximal n Frames (≤ 64), sodass auch n = 64 problemlos
 *        läuft. Jeder Aufruf von executeAndAdvance() erzeugt exakt einen Zug.
 *
 *        Die vom Aufgabenblatt geforderten Methoden ziehe() und hanoi() sind
 *        als vollständige Implementierungen des rekursiven Algorithmus
 *        vorhanden. Sie operieren auf den internen Pflöcken und dienen der
 *        Demonstration; für die Animation wird der Lazy-Generator genutzt.
 */
class HanoiSpiel : public QObject
{
    Q_OBJECT

public:
    /*
     * Autor: Henry Mirus
     * Eingabe: parent – übergeordnetes QObject (optional)
     * Rückgabe: –
     * Seiteneffekte: –
     */
    explicit HanoiSpiel(QObject *parent = nullptr);

    // ----------------------------------------------------------------
    // Aufbau und Rücksetzung
    // ----------------------------------------------------------------

    /*
     * Autor: Henry Mirus
     * Eingabe: scheibenzahl – Anzahl der Scheiben (1–64)
     * Rückgabe: –
     * Seiteneffekte: Erzeugt n goldene Scheiben (unterschiedliche Helligkeit),
     *                stapelt sie auf Pflock 0 und initialisiert den Lazy-Generator.
     */
    void initialisiere(int scheibenzahl);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Setzt alle drei Pflöcke auf den Anfangszustand zurück
     *                (alle Scheiben auf Pflock 0) und startet den Generator neu.
     */
    void setzeZurueck();

    // ----------------------------------------------------------------
    // Vom Aufgabenblatt geforderte Methoden (Aufgabe 1)
    // ----------------------------------------------------------------

    /*
     * Autor: Henry Mirus
     * Eingabe: start – Quellpflock-Index (0–2)
     *          ziel  – Zielpflock-Index  (0–2)
     *          i     – Zugnummer (informativer Parameter)
     * Rückgabe: –
     * Seiteneffekte: Verschiebt die oberste Scheibe von m_pegs[start] auf
     *                m_pegs[ziel].
     */
    void ziehe(int start, int ziel, int i);

    /*
     * Autor: Henry Mirus
     * Eingabe: scheibenzahl – Anzahl der noch zu versetzenden Scheiben
     *          start        – Quellpflock-Index
     *          ziel         – Zielpflock-Index
     *          zugnr        – laufende Zugnummer (Referenz, wird erhöht)
     * Rückgabe: –
     * Seiteneffekte: Löst das Turm-von-Hanoi-Problem rekursiv:
     *                1. n−1 Scheiben von start → Hilfspflock
     *                2. n-te Scheibe   von start → ziel  (ziehe)
     *                3. n−1 Scheiben  vom Hilfspflock → ziel
     */
    void hanoi(int scheibenzahl, int start, int ziel, int &zugnr);

    // ----------------------------------------------------------------
    // Lazy-Generator-Interface (für die Animation)
    // ----------------------------------------------------------------

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: true, wenn noch mindestens ein Zug aussteht
     * Seiteneffekte: –
     */
    bool hatWeitereZuege() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Quellpflock des nächsten anstehenden Zugs (−1 falls keiner)
     * Seiteneffekte: –
     */
    int currentMoveVon() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Zielpflock des nächsten anstehenden Zugs (−1 falls keiner)
     * Seiteneffekte: –
     */
    int currentMoveNach() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Führt den anstehenden Zug auf den internen Pflöcken aus
     *                und bereitet den nächsten Zug vor (Lazy-Advance).
     */
    void executeAndAdvance();

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Gesamtanzahl der Züge = 2^n − 1
     * Seiteneffekte: –
     */
    quint64 gesamtzuege() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Anzahl der bisher ausgeführten Züge
     * Seiteneffekte: –
     */
    quint64 zugzaehler() const;

    // ----------------------------------------------------------------
    // Abfragen
    // ----------------------------------------------------------------

    /*
     * Autor: Henry Mirus
     * Eingabe: index – Pflock-Index (0–2)
     * Rückgabe: Konstante Referenz auf den Pflock
     * Seiteneffekte: –
     */
    const Pflock &pflock(int index) const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Initialer Scheibenstapel (Index 0 = größte Scheibe)
     *           – nützlich, um den Endzustand direkt zu konstruieren
     * Seiteneffekte: –
     */
    const QVector<Scheibe> &scheibenliste() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Anzahl der Scheiben
     * Seiteneffekte: –
     */
    int scheibenzahl() const;

private:
    // ----- Lazy-Generator-Stack -----
    /*
     * Autor: Henry Mirus
     * Zweck: Ein Frame im expliziten Rekursionsstack des Lazy-Generators.
     *        phase 0 = vor erstem Rekursionsaufruf
     *        phase 1 = bereit, den eigentlichen Zug auszuführen
     *        phase 2 = nach dem Zug, vor zweitem Rekursionsaufruf
     */
    struct Frame { int n, src, dst, hilf, phase; };

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Setzt den Generator-Stack zurück und berechnet den
     *                ersten anstehenden Zug (prepareNextMove).
     */
    void initGenerator();

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: true, wenn ein Zug gefunden und in m_currentMoveVon/Nach
     *           gespeichert wurde
     * Seiteneffekte: Schreitet den Stack voran, bis phase == 1 gefunden
     *                (oder der Stack leer ist).
     */
    bool prepareNextMove();

    /*
     * Autor: Henry Mirus
     * Eingabe: index  – 0-basierter Index der Scheibe (0 = größte)
     *          gesamt – Gesamtanzahl der Scheiben
     * Rückgabe: Gold-Farbe; Helligkeit variiert von dunkel (größte) bis hell
     *           (kleinste Scheibe), Farbton konstant bei ca. 45° (Goldgelb)
     * Seiteneffekte: –
     */
    static QColor scheibenfarbe(int index, int gesamt);

    Pflock           m_pegs[3];        // Drei Pflöcke des Spiels
    QVector<Scheibe> m_scheiben;       // Initiale Scheibenkonfiguration
    int              m_scheibenzahl = 0;

    QVector<Frame>   m_genStack;       // Expliziter Rekursionsstack (max. n Frames)
    int              m_currentMoveVon  = -1;
    int              m_currentMoveNach = -1;
    bool             m_hasCurrent      = false;
    quint64          m_zugzaehler      = 0;
};

#endif // HANOI_H
