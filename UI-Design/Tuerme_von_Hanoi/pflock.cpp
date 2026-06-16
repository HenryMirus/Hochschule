#include "pflock.h"

/*
 * Autor: Henry Mirus
 * Eingabe: scheibe – aufzulegende Scheibe
 * Rückgabe: –
 * Seiteneffekte: Hängt scheibe am Ende des internen Vektors an (= oben auf Stapel).
 */
void Pflock::schiebeDrauf(const Scheibe &scheibe)
{
    m_scheiben.append(scheibe);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Kopie der zuletzt aufgelegten Scheibe
 * Seiteneffekte: Entfernt das letzte Element des internen Vektors.
 */
Scheibe Pflock::nehmeAb()
{
    return m_scheiben.takeLast();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Konstante Referenz auf die oberste (zuletzt hinzugefügte) Scheibe
 * Seiteneffekte: –
 */
const Scheibe &Pflock::oben() const
{
    return m_scheiben.last();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzahl der Scheiben auf dem Pflock
 * Seiteneffekte: –
 */
int Pflock::anzahl() const
{
    return m_scheiben.size();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: true, wenn der Stapel keine Scheiben enthält
 * Seiteneffekte: –
 */
bool Pflock::istLeer() const
{
    return m_scheiben.isEmpty();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Konstanter Verweis auf den internen Scheibenstapel
 * Seiteneffekte: –
 */
const QVector<Scheibe> &Pflock::scheiben() const
{
    return m_scheiben;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Entleert den internen Vektor.
 */
void Pflock::leere()
{
    m_scheiben.clear();
}
