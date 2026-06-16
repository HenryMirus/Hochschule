#ifndef SCHEIBE_H
#define SCHEIBE_H

#include <QColor>

/*
 * Autor: Henry Mirus
 * Zweck: Repräsentiert eine einzelne Scheibe der Türme-von-Hanoi.
 *        Eine Scheibe ist durch ihren Durchmesser eindeutig definiert
 *        und besitzt eine Farbe, durch die sie sich optisch von anderen
 *        Scheiben unterscheidet.
 */
class Scheibe
{
public:
    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Erzeugt eine ungültige Scheibe mit Durchmesser 0.
     *                Nötig für die Verwendung in Qt-Containern.
     */
    Scheibe() = default;

    /*
     * Autor: Henry Mirus
     * Eingabe: durchmesser – ganzzahliger Durchmesser der Scheibe (> 0)
     *          farbe       – Anzeigefarbe der Scheibe
     * Rückgabe: –
     * Seiteneffekte: –
     */
    Scheibe(int durchmesser, const QColor &farbe);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Durchmesser der Scheibe
     * Seiteneffekte: –
     */
    int durchmesser() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Anzeigefarbe der Scheibe
     * Seiteneffekte: –
     */
    QColor farbe() const;

private:
    int    m_durchmesser = 0;
    QColor m_farbe;
};

#endif // SCHEIBE_H
