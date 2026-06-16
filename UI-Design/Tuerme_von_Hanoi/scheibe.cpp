#include "scheibe.h"

/*
 * Autor: Henry Mirus
 * Eingabe: durchmesser – Durchmesser der Scheibe (> 0)
 *          farbe       – Anzeigefarbe der Scheibe
 * Rückgabe: –
 * Seiteneffekte: –
 */
Scheibe::Scheibe(int durchmesser, const QColor &farbe)
    : m_durchmesser(durchmesser)
    , m_farbe(farbe)
{
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Ganzzahliger Durchmesser der Scheibe
 * Seiteneffekte: –
 */
int Scheibe::durchmesser() const
{
    return m_durchmesser;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzeigefarbe der Scheibe
 * Seiteneffekte: –
 */
QColor Scheibe::farbe() const
{
    return m_farbe;
}
