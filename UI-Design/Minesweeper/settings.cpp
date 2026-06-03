#include "settings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: true, wenn rows > 0, columns > 0, mines >= 0 und mines < rows*columns
 * Seiteneffekte: –
 */
bool GameSettings::isValid() const
{
	return rows > 0 && columns > 0 && mines >= 0 && mines < rows * columns;
}

/*
 * Autor: Henry Mirus
 * Eingabe: other – zu vergleichende GameSettings-Instanz
 * Rückgabe: true, wenn Zeilen, Spalten und Minenanzahl übereinstimmen
 * Seiteneffekte: –
 */
bool GameSettings::operator==(const GameSettings &other) const
{
	return rows == other.rows && columns == other.columns && mines == other.mines;
}

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
 * Eingabe: object   – JSON-Objekt, in dem gesucht wird
 *          keys     – Liste von Schlüsselnamen (werden der Reihe nach versucht)
 *          fallback – Rückgabewert, wenn kein Schlüssel gefunden wird
 * Rückgabe: Ganzzahliger Wert des ersten gefundenen Schlüssels, sonst fallback
 * Seiteneffekte: –
 */
int readIntValue(const QJsonObject &object, std::initializer_list<const char *> keys, int fallback = 0)
{
	for (const char *key : keys) {
		const QJsonValue value = object.value(QLatin1String(key));
		if (value.isDouble()) {
			return value.toInt(fallback);
		}
	}

	return fallback;
}
} // namespace

namespace Settings
{
/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Absoluter Dateipfad zur settings.json im Programmverzeichnis
 * Seiteneffekte: –
 */
QString settingsFilePath()
{
	return QDir(dataDirectory()).filePath(QStringLiteral("settings.json"));
}

/*
 * Autor: Henry Mirus
 * Eingabe: filePath – optionaler Dateipfad; leer = Standardpfad (settings.json)
 * Rückgabe: Geladene GameSettings; leere Instanz bei Fehler oder ungültigen Daten
 * Seiteneffekte: –
 */
GameSettings loadSettings(const QString &filePath)
{
	QFile file(filePath.isEmpty() ? settingsFilePath() : filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		return {};
	}

	const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
	if (!document.isObject()) {
		return {};
	}

	const QJsonObject object = document.object();
	GameSettings settings;
	settings.rows = readIntValue(object, {"rows", "n", "height"});
	settings.columns = readIntValue(object, {"columns", "cols", "m", "width"});
	settings.mines = readIntValue(object, {"mines", "k"});

	if (!settings.isValid()) {
		return {};
	}

	return settings;
}

/*
 * Autor: Henry Mirus
 * Eingabe: settings – zu speichernde Spielkonfiguration
 *          filePath – optionaler Dateipfad; leer = Standardpfad (settings.json)
 * Rückgabe: true bei erfolgreichem Speichern, false bei Fehler oder ungültigen Daten
 * Seiteneffekte: Schreibt bzw. überschreibt die JSON-Datei auf dem Datenträger.
 */
bool saveSettings(const GameSettings &settings, const QString &filePath)
{
	if (!settings.isValid()) {
		return false;
	}

	QSaveFile file(filePath.isEmpty() ? settingsFilePath() : filePath);
	QDir().mkpath(QFileInfo(file.fileName()).absolutePath());
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}

	QJsonObject object;
	object.insert(QStringLiteral("rows"), settings.rows);
	object.insert(QStringLiteral("columns"), settings.columns);
	object.insert(QStringLiteral("mines"), settings.mines);

	file.write(QJsonDocument(object).toJson(QJsonDocument::Indented));
	return file.commit();
}
} // namespace Settings
