#ifndef STATISTICS_H
#define STATISTICS_H

#include "settings.h"

#include <QString>
#include <QStringList>
#include <QVector>

/*
 * Autor: Henry Mirus
 * Zweck: Speichert die aggregierten Spielstatistiken für eine bestimmte
 *        Spielkonfiguration (Zeilen, Spalten, Minenanzahl).
 */
struct StatisticEntry
{
	GameSettings settings;
	int gamesPlayed = 0;
	int wins = 0;
	int losses = 0;
	int bestWinTimeSeconds = -1;
};

/*
 * Autor: Henry Mirus
 * Zweck: Funktionen zum Laden, Speichern und Formatieren der Spielstatistiken
 *        als JSON-Datei im Programmverzeichnis.
 */
namespace Statistics
{
QString statisticsFilePath();
bool saveGame(bool won, int elapsedSeconds, const QString &filePath = QString());
QVector<StatisticEntry> loadStatistics(const QString &filePath = QString());
QString formatStatisticEntry(const StatisticEntry &entry);
QStringList loadStatisticsAsStrings(const QString &filePath = QString());
}

#endif // STATISTICS_H
