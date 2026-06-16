#ifndef HANOIEINSTELLUNGEN_H
#define HANOIEINSTELLUNGEN_H

/*
 * Autor: Henry Mirus
 * Zweck: Datenstruktur für die Konfigurationsparameter der Türme-von-Hanoi-
 *        Anwendung. Wird zwischen MainWindow, EinstellungsDialog und
 *        HanoiWidget ausgetauscht.
 */
struct HanoiEinstellungen
{
    int scheibenzahl = 5;   // Anzahl der Scheiben (1–12)
    int zugdauerMs   = 800; // Dauer eines Animationszugs in Millisekunden
};

#endif // HANOIEINSTELLUNGEN_H
