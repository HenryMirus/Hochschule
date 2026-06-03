#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

/*
 * Autor: Henry Mirus
 * Zweck: Speichert die Spielkonfiguration (Zeilen, Spalten, Minenanzahl)
 *        und stellt Validierung sowie Vergleich bereit.
 */
struct GameSettings
{
	int rows = 0;
	int columns = 0;
	int mines = 0;

	bool isValid() const;
	bool operator==(const GameSettings &other) const;
};

/*
 * Autor: Henry Mirus
 * Zweck: Funktionen zum Laden und Speichern der Spieleinstellungen als
 *        JSON-Datei im Programmverzeichnis.
 */
namespace Settings
{
QString settingsFilePath();
GameSettings loadSettings(const QString &filePath = QString());
bool saveSettings(const GameSettings &settings, const QString &filePath = QString());
}

#endif // SETTINGS_H
