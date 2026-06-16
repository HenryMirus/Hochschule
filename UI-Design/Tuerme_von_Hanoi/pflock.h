#ifndef PFLOCK_H
#define PFLOCK_H

#include <QVector>
#include "scheibe.h"

/*
 * Autor: Henry Mirus
 * Zweck: Repräsentiert einen Pflock (Stab) im Turm-von-Hanoi-Spiel.
 *        Verwaltet einen Stapel von Scheiben (Index 0 = unterste Scheibe).
 *        Bietet Operationen zum Auflegen, Abnehmen und Inspizieren von Scheiben.
 */
class Pflock
{
public:
    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Erzeugt einen leeren Pflock.
     */
    Pflock() = default;

    /*
     * Autor: Henry Mirus
     * Eingabe: scheibe – Scheibe, die oben auf den Pflock gelegt wird
     * Rückgabe: –
     * Seiteneffekte: Fügt scheibe am oberen Ende (Ende des internen Vektors) ein.
     */
    void schiebeDrauf(const Scheibe &scheibe);

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Kopie der obersten Scheibe
     * Seiteneffekte: Entfernt die oberste Scheibe aus dem internen Stapel.
     */
    Scheibe nehmeAb();

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Konstante Referenz auf die oberste Scheibe (ohne Entfernen)
     * Seiteneffekte: –
     */
    const Scheibe &oben() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Anzahl der Scheiben auf dem Pflock
     * Seiteneffekte: –
     */
    int anzahl() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: true, wenn keine Scheiben auf dem Pflock liegen
     * Seiteneffekte: –
     */
    bool istLeer() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: Konstante Referenz auf den internen Stapelvektor
     *           (Index 0 = unterste Scheibe, letzter Index = oberste Scheibe)
     * Seiteneffekte: –
     */
    const QVector<Scheibe> &scheiben() const;

    /*
     * Autor: Henry Mirus
     * Eingabe: –
     * Rückgabe: –
     * Seiteneffekte: Entfernt alle Scheiben vom Pflock.
     */
    void leere();

private:
    QVector<Scheibe> m_scheiben; // Stapel: Index 0 = unterste, last = oberste Scheibe
};

#endif // PFLOCK_H
