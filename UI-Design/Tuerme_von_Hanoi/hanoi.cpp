#include "hanoi.h"

/*
 * Autor: Henry Mirus
 * Eingabe: parent – übergeordnetes QObject (optional)
 * Rückgabe: –
 * Seiteneffekte: –
 */
HanoiSpiel::HanoiSpiel(QObject *parent)
    : QObject(parent)
{
}

/*
 * Autor: Henry Mirus
 * Eingabe: scheibenzahl – Anzahl der Scheiben (1–64)
 * Rückgabe: –
 * Seiteneffekte: Erzeugt n Scheiben im gewählten Farbschema, stapelt sie
 *                auf Pflock 0 und initialisiert den Lazy-Generator.
 */
void HanoiSpiel::initialisiere(int scheibenzahl, Farbschema farbschema)
{
    m_scheibenzahl = scheibenzahl;
    m_scheiben.clear();

    // Index 0 = größte Scheibe (Durchmesser n), Index n-1 = kleinste (d = 1)
    for (int i = 0; i < scheibenzahl; ++i) {
        const int d = scheibenzahl - i;
        m_scheiben.append(Scheibe(d, scheibenfarbe(i, scheibenzahl, farbschema)));
    }

    for (int i = 0; i < 3; ++i) {
        m_pegs[i].leere();
    }
    for (const Scheibe &s : m_scheiben) {
        m_pegs[0].schiebeDrauf(s);
    }

    initGenerator();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Setzt alle drei Pflöcke auf den Anfangszustand zurück
 *                und startet den Generator neu.
 */
void HanoiSpiel::setzeZurueck()
{
    for (int i = 0; i < 3; ++i) {
        m_pegs[i].leere();
    }
    for (const Scheibe &s : m_scheiben) {
        m_pegs[0].schiebeDrauf(s);
    }
    initGenerator();
}

/*
 * Autor: Henry Mirus
 * Eingabe: start – Quellpflock-Index (0–2)
 *          ziel  – Zielpflock-Index  (0–2)
 *          i     – Zugnummer (informativer Parameter, wird nicht gespeichert)
 * Rückgabe: –
 * Seiteneffekte: Verschiebt die oberste Scheibe von m_pegs[start] auf m_pegs[ziel].
 */
void HanoiSpiel::ziehe(int start, int ziel, int i)
{
    Q_UNUSED(i)
    if (!m_pegs[start].istLeer()) {
        Scheibe s = m_pegs[start].nehmeAb();
        m_pegs[ziel].schiebeDrauf(s);
    }
}

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
void HanoiSpiel::hanoi(int scheibenzahl, int start, int ziel, int &zugnr)
{
    if (scheibenzahl <= 0) {
        return;
    }
    const int hilf = 3 - start - ziel;
    hanoi(scheibenzahl - 1, start, hilf, zugnr);
    ziehe(start, ziel, zugnr++);
    hanoi(scheibenzahl - 1, hilf, ziel, zugnr);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: true, wenn noch mindestens ein Zug aussteht
 * Seiteneffekte: –
 */
bool HanoiSpiel::hatWeitereZuege() const
{
    return m_hasCurrent;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Quellpflock des nächsten anstehenden Zugs (−1 falls keiner)
 * Seiteneffekte: –
 */
int HanoiSpiel::currentMoveVon() const
{
    return m_currentMoveVon;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Zielpflock des nächsten anstehenden Zugs (−1 falls keiner)
 * Seiteneffekte: –
 */
int HanoiSpiel::currentMoveNach() const
{
    return m_currentMoveNach;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Führt den anstehenden Zug auf den internen Pflöcken aus,
 *                erhöht m_zugzaehler und bereitet den nächsten Zug vor.
 */
void HanoiSpiel::executeAndAdvance()
{
    if (!m_hasCurrent) {
        return;
    }

    if (!m_pegs[m_currentMoveVon].istLeer()) {
        Scheibe s = m_pegs[m_currentMoveVon].nehmeAb();
        m_pegs[m_currentMoveNach].schiebeDrauf(s);
    }
    ++m_zugzaehler;

    if (!m_genStack.isEmpty()) {
        m_genStack.last().phase = 2;
    }
    prepareNextMove();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Gesamtanzahl der Züge = 2^n − 1 (für n ≥ 64: UINT64_MAX)
 * Seiteneffekte: –
 */
quint64 HanoiSpiel::gesamtzuege() const
{
    if (m_scheibenzahl <= 0) {
        return 0;
    }
    if (m_scheibenzahl >= 64) {
        return ~quint64(0);
    }
    return (quint64(1) << m_scheibenzahl) - 1;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzahl der bisher ausgeführten Züge
 * Seiteneffekte: –
 */
quint64 HanoiSpiel::zugzaehler() const
{
    return m_zugzaehler;
}

/*
 * Autor: Henry Mirus
 * Eingabe: index – Pflock-Index (0–2)
 * Rückgabe: Konstante Referenz auf den Pflock
 * Seiteneffekte: –
 */
const Pflock &HanoiSpiel::pflock(int index) const
{
    return m_pegs[index];
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Initiale Scheibenkonfiguration (Index 0 = größte Scheibe)
 * Seiteneffekte: –
 */
const QVector<Scheibe> &HanoiSpiel::scheibenliste() const
{
    return m_scheiben;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzahl der Scheiben
 * Seiteneffekte: –
 */
int HanoiSpiel::scheibenzahl() const
{
    return m_scheibenzahl;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Setzt den Generator-Stack zurück, bereitet den ersten Zug vor.
 */
void HanoiSpiel::initGenerator()
{
    m_genStack.clear();
    m_zugzaehler      = 0;
    m_currentMoveVon  = -1;
    m_currentMoveNach = -1;
    m_hasCurrent      = false;

    if (m_scheibenzahl > 0) {
        // Startrahmen: verschiebe n Scheiben von Pflock 0 nach Pflock 2 via Pflock 1
        m_genStack.append({m_scheibenzahl, 0, 2, 1, 0});
        prepareNextMove();
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: true, wenn ein Zug gefunden und in m_currentMoveVon/Nach gespeichert wurde
 * Seiteneffekte: Schreitet den Stack vor, bis phase == 1 gefunden (oder Stack leer).
 *
 *                Phasen je Frame:
 *                  0 – neu: linke Subrekursion anstoßen, dann auf phase 1 setzen
 *                  1 – bereit: dieser Rahmen repräsentiert den eigentlichen Zug
 *                  2 – erledigt: rechte Subrekursion anstoßen, Rahmen entfernen
 */
bool HanoiSpiel::prepareNextMove()
{
    m_hasCurrent = false;

    while (!m_genStack.isEmpty()) {
        if (m_genStack.last().phase == 0) {
            // Werte vor möglicher Reallokation durch append sichern
            const int n    = m_genStack.last().n;
            const int src  = m_genStack.last().src;
            const int dst  = m_genStack.last().dst;
            const int hilf = m_genStack.last().hilf;
            m_genStack.last().phase = 1;
            if (n > 1) {
                // Linke Subrekursion: n-1 Scheiben von src nach hilf
                m_genStack.append({n - 1, src, hilf, dst, 0});
            }

        } else if (m_genStack.last().phase == 1) {
            m_currentMoveVon  = m_genStack.last().src;
            m_currentMoveNach = m_genStack.last().dst;
            m_hasCurrent      = true;
            return true;

        } else { // phase == 2
            const int n    = m_genStack.last().n;
            const int hilf = m_genStack.last().hilf;
            const int dst  = m_genStack.last().dst;
            m_genStack.removeLast();
            if (n > 1) {
                // Rechte Subrekursion: n-1 Scheiben von hilf nach dst
                m_genStack.append({n - 1, hilf, dst, 3 - hilf - dst, 0});
            }
        }
    }

    return false;
}

/*
 * Autor: Henry Mirus
 * Eingabe: index  – 0-basierter Index der Scheibe (0 = größte)
 *          gesamt – Gesamtanzahl der Scheiben
 * Rückgabe: Scheibenfarbe; Gold: Helligkeit von dunkel (größte) bis hell (kleinste);
 *           Regenbogen: Farbton gleichmäßig auf dem Farbkreis verteilt.
 * Seiteneffekte: –
 */
QColor HanoiSpiel::scheibenfarbe(int index, int gesamt, Farbschema farbschema)
{
    if (gesamt <= 1) {
        return farbschema == Farbschema::Gold
               ? QColor::fromHsvF(45.0 / 360.0, 0.90, 1.0)
               : QColor::fromHsvF(0.0, 0.85, 0.90);
    }

    if (farbschema == Farbschema::Regenbogen) {
        // Farbton gleichmäßig über den Farbkreis verteilt (0 = größte, gesamt-1 = kleinste)
        const qreal hue = qreal(index) / qreal(gesamt);
        return QColor::fromHsvF(hue, 0.85, 0.90);
    }

    // Gold: Helligkeit von dunkelgold (größte) bis hellgold (kleinste)
    const qreal value = 0.55 + 0.45 * (qreal(index) / qreal(gesamt - 1));
    return QColor::fromHsvF(45.0 / 360.0, 0.90, value);
}
