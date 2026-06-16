#ifndef HANOIEINSTELLUNGEN_H
#define HANOIEINSTELLUNGEN_H

/*
 * Autor: Henry Mirus
 * Zweck: Farbschema-Auswahl für die Scheibendarstellung.
 *        Gold      – alle Scheiben goldfarben (Helligkeit variiert von dunkel
 *                    bei der größten bis hell bei der kleinsten Scheibe)
 *        Regenbogen – Scheiben gleichmäßig auf dem Farbkreis verteilt
 */
enum class Farbschema
{
    Gold,
    Regenbogen
};

/*
 * Autor: Henry Mirus
 * Zweck: Datenstruktur für die Konfigurationsparameter der Türme-von-Hanoi-
 *        Anwendung. Wird zwischen MainWindow, EinstellungsDialog und
 *        HanoiWidget ausgetauscht.
 */
struct HanoiEinstellungen
{
    int        scheibenzahl = 5;                // Anzahl der Scheiben (1–64)
    int        zugdauerMs   = 800;              // Dauer eines Animationszugs in ms
    Farbschema farbschema   = Farbschema::Gold; // Farbschema der Scheiben
};

#endif // HANOIEINSTELLUNGEN_H
