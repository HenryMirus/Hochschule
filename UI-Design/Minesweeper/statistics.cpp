#include "statistics.h"

#include <algorithm>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace
{
/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Absoluter Pfad zum Datenverzeichnis (Programmverzeichnis oder
 *           aktuelles Verzeichnis als Fallback)
 * Seiteneffekte: Legt das Verzeichnis an, falls es nicht existiert.
 */
QString dataDirectory()
{
	QString location = QCoreApplication::applicationDirPath();
	if (location.isEmpty()) {
		location = QDir::currentPath();
	}

	QDir directory(location);
	if (!directory.exists()) {
		directory.mkpath(QStringLiteral("."));
	}

	return directory.absolutePath();
}

/*
 * Autor: Henry Mirus
 * Eingabe: filePath    – bevorzugter Pfad; wenn leer, wird defaultName verwendet
 *          defaultName – Dateiname, der im Datenverzeichnis gesucht wird
 * Rückgabe: Aufgelöster absoluter Dateipfad
 * Seiteneffekte: –
 */
QString resolvedPath(const QString &filePath, const QString &defaultName)
{
	return filePath.isEmpty() ? QDir(dataDirectory()).filePath(defaultName) : filePath;
}

/*
 * Autor: Henry Mirus
 * Eingabe: settings – zu serialisierende Spielkonfiguration
 * Rückgabe: JSON-Objekt mit den Feldern rows, columns, mines sowie den
 *           Kurzbezeichnungen n, m, k
 * Seiteneffekte: –
 */
QJsonObject settingsToJson(const GameSettings &settings)
{
	QJsonObject object;
	object.insert(QStringLiteral("rows"), settings.rows);
	object.insert(QStringLiteral("columns"), settings.columns);
	object.insert(QStringLiteral("mines"), settings.mines);
	object.insert(QStringLiteral("n"), settings.rows);
	object.insert(QStringLiteral("m"), settings.columns);
	object.insert(QStringLiteral("k"), settings.mines);
	return object;
}

/*
 * Autor: Henry Mirus
 * Eingabe: object – JSON-Objekt mit Spielkonfigurationsdaten
 * Rückgabe: GameSettings, befüllt aus den vorhandenen Schlüsseln;
 *           ungültige Felder bleiben 0
 * Seiteneffekte: –
 */
GameSettings settingsFromJson(const QJsonObject &object)
{
	GameSettings settings;
	settings.rows = object.value(QStringLiteral("rows")).toInt(object.value(QStringLiteral("n")).toInt(0));
	if (settings.rows <= 0) {
		settings.rows = object.value(QStringLiteral("height")).toInt(0);
	}

	settings.columns = object.value(QStringLiteral("columns")).toInt(object.value(QStringLiteral("cols")).toInt(0));
	if (settings.columns <= 0) {
		settings.columns = object.value(QStringLiteral("m")).toInt(object.value(QStringLiteral("width")).toInt(0));
	}

	settings.mines = object.value(QStringLiteral("mines")).toInt(object.value(QStringLiteral("k")).toInt(0));
	return settings;
}

/*
 * Autor: Henry Mirus
 * Eingabe: entry – zu serialisierender Statistikeintrag
 * Rückgabe: JSON-Objekt mit allen Feldern des Eintrags
 * Seiteneffekte: –
 */
QJsonObject entryToJson(const StatisticEntry &entry)
{
	QJsonObject object;
	object.insert(QStringLiteral("settings"), settingsToJson(entry.settings));
	object.insert(QStringLiteral("gamesPlayed"), entry.gamesPlayed);
	object.insert(QStringLiteral("wins"), entry.wins);
	object.insert(QStringLiteral("losses"), entry.losses);
	object.insert(QStringLiteral("bestWinTimeSeconds"), entry.bestWinTimeSeconds);
	return object;
}

/*
 * Autor: Henry Mirus
 * Eingabe: object – JSON-Objekt mit Statistikdaten eines Eintrags
 * Rückgabe: StatisticEntry, befüllt aus den vorhandenen Schlüsseln
 * Seiteneffekte: –
 */
StatisticEntry entryFromJson(const QJsonObject &object)
{
	StatisticEntry entry;
	entry.settings = settingsFromJson(object.value(QStringLiteral("settings")).toObject());
	if (!entry.settings.isValid()) {
		entry.settings = settingsFromJson(object);
	}

	entry.gamesPlayed = object.value(QStringLiteral("gamesPlayed")).toInt(0);
	entry.wins = object.value(QStringLiteral("wins")).toInt(0);
	entry.losses = object.value(QStringLiteral("losses")).toInt(0);
	entry.bestWinTimeSeconds = object.value(QStringLiteral("bestWinTimeSeconds")).toInt(-1);
	return entry;
}

/*
 * Autor: Henry Mirus
 * Eingabe: filePath – Pfad zur JSON-Statistikdatei
 * Rückgabe: Liste aller gültigen Statistikeinträge, sortiert nach
 *           Zeilen, dann Spalten, dann Minenanzahl
 * Seiteneffekte: –
 */
QVector<StatisticEntry> readEntries(const QString &filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		return {};
	}

	const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
	if (!document.isObject()) {
		return {};
	}

	const QJsonArray array = document.object().value(QStringLiteral("entries")).toArray();
	QVector<StatisticEntry> entries;
	entries.reserve(array.size());

	for (const QJsonValue &value : array) {
		if (value.isObject()) {
			const StatisticEntry entry = entryFromJson(value.toObject());
			if (entry.settings.isValid()) {
				entries.push_back(entry);
			}
		}
	}

	std::sort(entries.begin(), entries.end(), [](const StatisticEntry &left, const StatisticEntry &right) {
		if (left.settings.rows != right.settings.rows) {
			return left.settings.rows < right.settings.rows;
		}

		if (left.settings.columns != right.settings.columns) {
			return left.settings.columns < right.settings.columns;
		}

		return left.settings.mines < right.settings.mines;
	});

	return entries;
}

/*
 * Autor: Henry Mirus
 * Eingabe: filePath – Pfad zur JSON-Statistikdatei
 *          entries  – zu schreibende Statistikeinträge
 * Rückgabe: true bei erfolgreichem Schreiben, false bei Fehler
 * Seiteneffekte: Schreibt bzw. überschreibt die JSON-Datei auf dem Datenträger.
 */
bool writeEntries(const QString &filePath, const QVector<StatisticEntry> &entries)
{
	QFile file(filePath);
	QDir().mkpath(QFileInfo(file.fileName()).absolutePath());
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}

	QJsonArray array;
	for (const StatisticEntry &entry : entries) {
		array.append(entryToJson(entry));
	}

	QJsonObject root;
	root.insert(QStringLiteral("entries"), array);
	file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
	return true;
}
} // namespace

namespace Statistics
{
/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Absoluter Dateipfad zur statistics.json im Programmverzeichnis
 * Seiteneffekte: –
 */
QString statisticsFilePath()
{
	return QDir(dataDirectory()).filePath(QStringLiteral("statistics.json"));
}

/*
 * Autor: Henry Mirus
 * Eingabe: won            – true wenn das Spiel gewonnen wurde
 *          elapsedSeconds – Dauer des Spiels in Sekunden
 *          filePath       – optionaler Dateipfad; leer = Standardpfad
 * Rückgabe: true bei erfolgreichem Speichern, false bei Fehler
 * Seiteneffekte: Liest die aktuelle statistics.json, aktualisiert oder erstellt
 *                den Eintrag für die aktuelle Konfiguration und schreibt die
 *                Datei zurück.
 */
bool saveGame(bool won, int elapsedSeconds, const QString &filePath)
{
	const GameSettings currentSettings = Settings::loadSettings();
	if (!currentSettings.isValid()) {
		return false;
	}

	const QString targetPath = resolvedPath(filePath, QStringLiteral("statistics.json"));
	QVector<StatisticEntry> entries = readEntries(targetPath);

	auto it = std::find_if(entries.begin(), entries.end(), [&currentSettings](const StatisticEntry &entry) {
		return entry.settings == currentSettings;
	});

	if (it == entries.end()) {
		StatisticEntry entry;
		entry.settings = currentSettings;
		entry.gamesPlayed = 1;
		entry.wins = won ? 1 : 0;
		entry.losses = won ? 0 : 1;
		entry.bestWinTimeSeconds = won ? elapsedSeconds : -1;
		entries.push_back(entry);
		return writeEntries(targetPath, entries);
	}

	it->gamesPlayed += 1;
	if (won) {
		it->wins += 1;
		if (it->bestWinTimeSeconds < 0 || elapsedSeconds < it->bestWinTimeSeconds) {
			it->bestWinTimeSeconds = elapsedSeconds;
		}
	} else {
		it->losses += 1;
	}

	return writeEntries(targetPath, entries);
}

/*
 * Autor: Henry Mirus
 * Eingabe: filePath – optionaler Dateipfad; leer = Standardpfad
 * Rückgabe: Liste aller gespeicherten Statistikeinträge
 * Seiteneffekte: –
 */
QVector<StatisticEntry> loadStatistics(const QString &filePath)
{
	return readEntries(resolvedPath(filePath, QStringLiteral("statistics.json")));
}

/*
 * Autor: Henry Mirus
 * Eingabe: entry – formatierender Statistikeintrag
 * Rückgabe: Lesbare Zeichenkette mit Konfiguration, Spielanzahl, Siegen,
 *           Niederlagen und bester Gewinnzeit
 * Seiteneffekte: –
 */
QString formatStatisticEntry(const StatisticEntry &entry)
{
	const QString bestTimeText = entry.bestWinTimeSeconds >= 0
		? QString::number(entry.bestWinTimeSeconds) + QStringLiteral(" s")
		: QStringLiteral("n/a");

	return QStringLiteral("%1x%2 / %3 mines | games: %4 | wins: %5 | losses: %6 | best win: %7")
		.arg(entry.settings.rows)
		.arg(entry.settings.columns)
		.arg(entry.settings.mines)
		.arg(entry.gamesPlayed)
		.arg(entry.wins)
		.arg(entry.losses)
		.arg(bestTimeText);
}

/*
 * Autor: Henry Mirus
 * Eingabe: filePath – optionaler Dateipfad; leer = Standardpfad
 * Rückgabe: Liste aller Statistikeinträge als formatierte Zeichenketten
 * Seiteneffekte: –
 */
QStringList loadStatisticsAsStrings(const QString &filePath)
{
	const QVector<StatisticEntry> entries = loadStatistics(filePath);
	QStringList output;
	output.reserve(entries.size());

	for (const StatisticEntry &entry : entries) {
		output.push_back(formatStatisticEntry(entry));
	}

	return output;
}
} // namespace Statistics
