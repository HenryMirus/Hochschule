# Türme von Hanoi

Autor: Henry Mirus

## KI-Disclaimer
Claude Code wurden verwendet, um Funktionsbeschreibungen zu verfassen, Übersichtlichkeit in der Struktur des Skriptes zu gewährleisten und Verbesserungen (hinsichtlich Animationslogik und Speichereffizienz bei großen Scheibenanzahlen) einzubauen.

## Inhalt
Dieses Projekt implementiert eine animierte Visualisierung der Türme von Hanoi als Qt-Desktop-Anwendung mit konfigurierbarer Scheibenanzahl, einstellbarer Zuggeschwindigkeit und Pause-Funktion.

## Funktionen
- Visualisierung von 1 bis 64 Scheiben auf drei Pflöcken
- Wählbares Farbschema: Goldfarben (Standard) oder Regenbogen
- Flüssige Scheibenanimation entlang einer kubischen Bézier-Kurve mit Smoothstep-Geschwindigkeitsfunktion
- Einstellbare Zugdauer (100 ms bis 5000 ms)
- Steuerung über vier Schaltflächen: Neu, Start, Pause und Beenden
- Statuszeile mit Scheibenanzahl, Zugfortschritt und Animationszustand
- Blinkende Zustandsanzeige während der Pause
- Einstellungsdialog für Scheibenanzahl, Farbschema und Zugdauer
- Scheibengröße passt sich an, da bei vielen Scheiben das Fenster trotzdem zu klein ist
- Die Fenstergröße ist anpassbar
- Hintergrundfarbe folgt dem System-Theme (Light/Dark Mode)

## Funktionsweise
Beim Start wird ein neues Spiel mit den Standardeinstellungen (5 Scheiben, goldfarben, 800 ms pro Zug) aufgebaut. Die Scheiben werden auf dem linken Pflock gestapelt. Im goldenen Farbschema variiert die Helligkeit von der größten (dunkler) bis zur kleinsten (heller) Scheibe; im Regenbogen-Modus werden die Farbtöne gleichmäßig über den Farbkreis verteilt. Der Hintergrund passt sich automatisch dem Light- oder Dark-Mode des Betriebssystems an.

Die Lösung basiert auf dem rekursiven Hanoi-Algorithmus: n−1 Scheiben auf den Hilfspflock, dann die n-te Scheibe auf den Zielpflock, dann n−1 Scheiben auf den Zielpflock — insgesamt 2ⁿ−1 Züge. Um bis zu 64 Scheiben (18,4 Trillionen Züge) ohne Speicherproblem zu unterstützen, werden Züge nicht vorab berechnet, sondern durch einen Lazy-Generator mit einem Rekursionsstack von maximal n Einträgen on-demand erzeugt.

Jede Scheibe bewegt sich entlang einer kubischen Bézier-Kurve (Bogen über die Stabspitzen) mit Smoothstep-Geschwindigkeitsfunktion f(t) = t²·(3−2t) für sanftes Anfahren und Abbremsen. Die Zugdauer ist frei einstellbar.

Pause hält die Animation an und akkumuliert die verstrichene Zeit, sodass nach dem Fortsetzen nahtlos weitergemacht wird; die Statusleiste blinkt dabei im 500-ms-Takt. Beenden bricht die Animation ab und zeigt sofort den Endzustand (alle Scheiben auf dem rechten Pflock) an. Über den Einstellungsdialog lassen sich Scheibenanzahl und Zugdauer anpassen; nach Bestätigung wird automatisch ein neues Spiel generiert.

## Build
Das Projekt unterstützt zwei Build-Systeme:
- **CMake** (empfohlen für Qt Creator): Build über `CMakeLists.txt`
- **qmake** (für die Hochschul-Poolrechner unter Linux): Build über `TuermeVonHanoi.pro` mit `qmake` und `make`

## Dateien
- `mainwindow.*` für das Hauptfenster mit Schaltflächen, Statuszeile und Blink-Timer
- `hanoiwidget.*` für die grafische Darstellung der Pflöcke und Scheiben sowie die Animationssteuerung
- `hanoi.*` für die Spiellogik: rekursiver Algorithmus (`hanoi`, `ziehe`) und Lazy-Generator
- `pflock.*` für die Datenstruktur eines einzelnen Pflocks als Scheibenstapel
- `scheibe.*` für die Datenstruktur einer einzelnen Scheibe mit Durchmesser und Farbe
- `hanoieinstellungen.h` für die gemeinsame Einstellungsstruktur (Scheibenanzahl, Farbschema, Zugdauer)
- `einstellungsdialog.*` für den Einstellungsdialog (Scheibenanzahl, Farbschema, Zugdauer)
