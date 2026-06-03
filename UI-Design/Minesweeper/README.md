# Minesweeper

Autor: Henry Mirus

## KI-Disclaimer
Github-Copilot Pro und Claude Code wurden verwendet, um Funktionsbeschreibungen zu verfassen, Übersichtlichkeit in der Struktur des Skriptes zu gewährleisten und Verbesserungen (hinsichtlich Exception-Handling und Edge-Cases) einzubauen.

## Inhalt
Dieses Projekt implementiert das klassische Minesweeper-Spiel als Qt-Desktop-Anwendung mit konfigurierbarem Spielfeld, Statistiken und Pausenfunktion.

## Funktionen
- Spielfeld mit frei wählbarer Zeilen- und Spaltenanzahl sowie Minenanzahl
- Felder aufdecken per Linksklick, Flagge setzen per Rechtsklick, Flagge entfernen per Mittelklick
- Automatisches Aufdecken zusammenhängender leerer Felder (Flood-Fill)
- Erster Klick ist stets sicher – Minen werden erst danach platziert
- Flaggen, Pause und manuelles Beenden nur möglich, nachdem das Spiel gestartet wurde
- Spieltimer mit Anzeige in der Statusleiste
- Gewinn- und Verlustmeldung per Dialogfenster
- Einstellungsdialog für Zeilen, Spalten und Minenanzahl
- Statistikanzeige mit Spielanzahl, Siegen, Niederlagen und bester Zeit
- Hilfe-Dialog mit Spielerklärung
- Einstellungen und Statistiken werden dauerhaft als JSON-Datei gespeichert

## Funktionsweise
Beim Start der Anwendung werden die zuletzt gespeicherten Einstellungen (Zeilen, Spalten, Minenanzahl) aus der Datei `settings.json` geladen. Sind noch keine Einstellungen vorhanden, startet das Spiel mit einem 9×9-Feld und 10 Minen. Das Spielfeld wird als Gitter aus gleichgroßen Schaltflächen aufgebaut und passt sich automatisch an die gewählte Feldgröße an.

Zu Beginn jedes Spiels sind alle Felder verdeckt. Minen werden noch nicht auf dem Spielfeld verteilt. Erst wenn der Spieler das erste Feld per Linksklick aufdeckt, werden die Minen zufällig platziert – dabei wird eine Schutzzone rund um das erste angeklickte Feld freigehalten, sodass der erste Zug nie in eine Mine führt. Gleichzeitig startet der Timer und die Schaltflächen für Pause und manuelles Beenden werden freigegeben. Vor dem ersten Klick sind diese Funktionen gesperrt, ebenso wie das Setzen von Flaggen.

Deckt ein Spieler ein leeres Feld ohne benachbarte Minen auf, werden alle direkt und indirekt angrenzenden leeren Felder automatisch mitaufgedeckt (Flood-Fill). Felder mit benachbarten Minen zeigen nach dem Aufdecken die entsprechende Anzahl an. Eine aufgedeckte Mine beendet das Spiel sofort als Niederlage und legt alle verbleibenden Minen offen. Das Spiel ist gewonnen, sobald alle minenfreien Felder aufgedeckt sind.

Über den Pause-Button lässt sich das Spiel unterbrechen. Dabei wird der Timer angehalten und das Spielfeld ausgeblendet, sodass keine Informationen sichtbar sind. Nach dem Fortsetzen wird das Feld wiederhergestellt und der Timer läuft weiter. Der Beenden-Button bricht das laufende Spiel manuell ab, ohne eine Gewinn- oder Verlustmeldung auszugeben.

Am Ende jedes Spiels – ob gewonnen, verloren oder durch Mine getroffen – wird das Ergebnis zusammen mit der Spielzeit in der Statistikdatei gespeichert. Die Statistik ist nach Spielfeldkonfiguration gruppiert und zeigt für jede Konfiguration die Gesamtanzahl der Spiele, Siege, Niederlagen sowie die beste Gewinnzeit an.

## Spielregeln
- Ziel ist es, alle Felder ohne Minen aufzudecken
- Zahlen auf aufgedeckten Feldern geben an, wie viele der bis zu acht Nachbarfelder eine Mine enthalten
- Verdächtige Felder können mit einer Flagge markiert werden (Rechtsklick), Flaggen lassen sich per Mittelklick wieder entfernen
- Eine Mine zu treffen beendet das Spiel sofort als Niederlage

## Build
Das Projekt ist ein Qt/CMake-Projekt. Der Build erfolgt in der vorhandenen Qt-Umgebung über CMake bzw. Qt Creator.

## Dateien
- `mainwindow.*` für das Hauptfenster, die Spielsteuerung und die Darstellung des Spielfelds
- `game.*` für die gesamte Spiellogik (Feldverwaltung, Aufdecken, Flaggen, Timer, Gewinn- und Verlusterkennung)
- `cellbutton.*` für die individuelle Schaltfläche jedes Spielfelds mit Links-, Rechts- und Mittelklick-Unterstützung
- `settings.*` für das Laden und Speichern der Spieleinstellungen als JSON
- `statistics.*` für das Laden und Speichern der Spielstatistiken als JSON
- `settingsdialog.*` für den Einstellungsdialog (Zeilen, Spalten, Minenanzahl)
- `statisticdialog.*` für die Anzeige der gespeicherten Spielstatistiken
- `helpdialog.*` für den Hilfe-Dialog mit Spielanleitung
