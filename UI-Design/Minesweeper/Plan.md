## Plan: Minesweeper Projekt (Qt/C++)

TL;DR - Was, warum, wie
Das Ziel ist, das bestehende Qt-Projekt `Minesweeper` zu vervollständigen: saubere Architektur, klare Klassen- und Funktionsnamen, vollständige Spiel-Logik (Board, Cell, Spielsteuerung), und ein fertiges Frontend mit genauen Anweisungen zum Nachbauen der Fenster in Qt Designer bzw. als QWidget-Implementierung. Ich empfehle eine Trennung in Model (Game, Board, Cell), View (CellWidget, MainWindow UI) und Controller (GameController / MainWindow glue). Diese Struktur ist wiederverwendbar, testbar und passt zur vorhandenen CMake/Qt-Konfiguration.

**Steps**
1. Anforderungen prüfen (manuell): Öffne `UI-Design/Minesweeper/u03.pdf` und extrahiere konkrete Aufgaben/Regeln (Board-Größen, Minenanzahl, Zusatzanforderungen). *abhängig: Benutzer*.
2. Architektur & Datenmodell: Definiere Klassen `Game`, `Board`, `Cell` und `HighscoreManager`. `Game` orchestriert, `Board` enthält `std::vector<std::vector<Cell>>`, `Cell` hält Zustand (isMine, isRevealed, isFlagged, adjacentMines).
3. UI-Design (Frontend): Entwurf von `MainWindow`-Layout und `CellWidget` (QPushButton-Subclass oder custom QWidget). Detaillierte Anleitung unten.
4. Implementiere Model-Methoden: `Board::placeMines(seed)`, `Board::revealCell(row,col)`, `Board::toggleFlag(row,col)`, flood-fill für leere Zellen, Sieg-/Niederlage-Logik.
5. Implementiere View & Verbindung: `CellWidget` sendet Signale `leftClicked(row,col)`, `rightClicked(row,col)`; `MainWindow` / `GameController` verbindet Signale mit Model-Methoden und reagiert auf Modell-Events (gameOver, gameWon, cellUpdated).
6. Status, Timer, Score: Timer (QTimer), MinesRemaining counter, und Statusbar-Nachrichten.
7. Dialoge & Extras: Neues-Spiel-Dialog (Größe/Minen), Highscore-Dialog, Einstellungen (optional), About.
8. Tests & Verifikation: Unit-Tests für `Board`-Funktionen (falls Test-Framework möglich), manuelle Testfälle (corner cases).
9. Dokumentation & Delivery: `Plan.md` (dieses Dokument), README mit Build/Run-Anleitung, ggf. Kommentierung in Dateien.

**Relevante Dateien**
- UI-Design/Minesweeper/main.cpp — App-Einstieg
- UI-Design/Minesweeper/mainwindow.h — vorhandene QMainWindow-Definition
- UI-Design/Minesweeper/mainwindow.cpp — vorhandene Implementation
- UI-Design/Minesweeper/mainwindow.ui — Qt Designer UI
- UI-Design/Minesweeper/CMakeLists.txt — Build
- UI-Design/Patientenverwaltung/ — Referenzarchitektur / Patterns

**Detailliertes Design — Architektur**
- `Game` (Klasse)
  - Verantwortung: Spielzustand, Start/Neustart, Sieg/Niederlage, Timer-Start/Stop
  - Methoden: `start(rows, cols, mines)`, `reveal(row,col) -> RevealResult`, `toggleFlag(row,col)`, `isRunning()`, `elapsedSeconds()`
  - Signale (bei Verwendung von QObject): `cellChanged(row,col)`, `gameLost()`, `gameWon()`, `minesCountChanged(int)`
- `Board` (Klasse, rein datenorientiert)
  - Attribute: `int rows, cols, mines; std::vector<std::vector<Cell>> cells; bool minesPlaced;`
  - Methoden: `placeMines(firstClickRow, firstClickCol)`, `countAdjacent(row,col)`, `inBounds(r,c)`
- `Cell` (struct / lightweight class)
  - Felder: `bool isMine; bool isRevealed; bool isFlagged; uint8_t adjacentMines;`
- `GameController` (optional, kann in `MainWindow` integriert werden)
  - Vermittler zwischen UI-Signalen und Game/Board-Methoden. Verantwortlich für Highscore-Aufruf, UI-Aktualisierung.
- `HighscoreManager` (optional)
  - Speichert lokale Highscores (z.B. JSON mit Qt `QFile` + `QJsonDocument`).

**Benennungskonventionen (Beispiele)**
- Klassen: `Game`, `Board`, `Cell`, `CellWidget`, `HighscoreManager`, `SettingsDialog` (PascalCase)
- Methoden: `startGame()`, `revealCell(int row, int col)`, `toggleFlag(int row, int col)` (camelCase)
- Signals/slots: `cellLeftClicked(int row, int col)`, `cellRightClicked(int row, int col)`, `onNewGameTriggered()`
- Variablen: `rows`, `cols`, `mineCount`, `elapsedSeconds`, `isRevealed`

**Frontend: genaue Anweisungen (Qt Designer / C++ Umsetzung)**
Ziel: Ein übersichtliches Hauptfenster mit Spielbrett, Steuerleiste und Statusanzeige.

MainWindow (QMainWindow) — Struktur
- Menü: `Spiel` → `Neu`, `Schwierigkeit`, `Beenden`; `Hilfe` → `Über`
- Toolbar (optional): Buttons `Neu`, `Zurücksetzen`, `Highscores`
- Zentral-Widget: Haupt-Layout (QVBoxLayout)
  - Oben: Steuerleiste (QHBoxLayout)
    - `QPushButton` `NewGameButton` (Text: "Neu")
    - `QLabel` `MinesLabel` (Text: "Minen: <count>")
    - `QLabel` `TimerLabel` (Text: "00:00")
    - `QComboBox` `DifficultyCombo` (Einträge: "Einfach", "Mittel", "Schwer", "Benutzerdefiniert")
  - Mitte: Board-Area (QScrollArea oder QWidget mit QGridLayout)
    - Widget: `BoardWidget` — enthält `QGridLayout` für Zellen
    - Zellen: `CellWidget` (empfohlen: `QPushButton`-Subclass) mit fixed size 24×24 oder 32×32 (je nach DPI)
  - Unten: `QStatusBar` für Hinweise (z.B. "Spiel läuft", "Verloren: Mine bei (r,c)")

CellWidget (Implementationshinweise)
- Empfehlung: Subclass von `QPushButton` oder `QToolButton`:
  - Konstruktor: `CellWidget(int row, int col, QWidget* parent = nullptr)`
  - Felder: `int row, col;`
  - Signale: `leftClicked(int row,int col)`, `rightClicked(int row,int col)`
  - Reimplementiere `mousePressEvent(QMouseEvent* e)`:
    - `if (e->button() == Qt::LeftButton) emit leftClicked(row,col);`
    - `else if (e->button() == Qt::RightButton) emit rightClicked(row,col);`
  - Darstellung: Nutze `setText()` für Anzeige der Zahlen, `setIcon()` für Flaggen und Mine (bei Explosion)
  - Styles: Verwende `setStyleSheet()` oder `QPalette` um unrevealed/revealed/flagged/exploded Zustände zu unterscheiden.

BoardWidget / Board-Layout
- Erstelle `BoardWidget : public QWidget` mit internem `QGridLayout* grid;`
- Beim Start: grid->addWidget(cellWidget, row, col);
- Bei veränderter Zelle: aktualisiere nur diese `CellWidget` (Performance)
- Bei großen Grids: Umwickle `BoardWidget` in `QScrollArea`.

Signal-Flow (Beispiel)
- `CellWidget::leftClicked(r,c)` -> connected to `MainWindow::onCellLeftClicked(r,c)`
- `MainWindow::onCellLeftClicked` -> `game.reveal(r,c)` -> `game` emits `cellChanged(r,c)` -> `MainWindow` updates `CellWidget`
- `CellWidget::rightClicked(r,c)` -> `MainWindow::onCellRightClicked` -> `game.toggleFlag(r,c)` -> `cellChanged` emitted

UI-Details (Design / Look)
- Unrevealed cells: grauer Hintergrund, kein Text
- Revealed empty: heller Hintergrund, Zahl in Farbe (1: blau, 2: grün, 3: rot, etc.)
- Flag: rotes Flaggen-Icon (kleines PNG in Ressourcen)
- Mine: schwarzer Punkt oder Bomben-Icon bei Verlust
- Responsive spacing: `CellWidget->setFixedSize(32,32)`; Grid spacing 2 px
- Schrift: bold, zentriert, QFont size ~10-14 abhängig von cell size

Dialogs
- `NewGameDialog` (QDialog) — Felder: rows (QSpinBox), cols (QSpinBox), mines (QSpinBox), seed optional
- `HighscoreDialog` — QTableView / QListWidget mit `name/time/rows/cols/mines`
- `GameOverDialog` — zeigt Ergebnis, Zeit und Button für neues Spiel

**Implementations-Schritte (technisch, in Reihenfolge)**
1. Model-Klassen erstellen: `src/game.h/.cpp`, `src/board.h/.cpp`, `src/cell.h`.
2. `CellWidget` erstellen: `ui/cellwidget.h/.cpp` oder inline in `BoardWidget`.
3. `BoardWidget` erstellen und in `mainwindow.ui` Platzhalter-Widget (QWidget, objectName: `boardContainer`) setzen.
4. Verbindungen: In `MainWindow` `setupUi(this)` und danach `board = new BoardWidget(...); ui->centralwidgetLayout->addWidget(board);` und connect-Slots.
5. Implementiere `placeMines` beim ersten Klick (vermeidet Mine auf erster Klick).
6. Timer & MinesRemaining: `QTimer* gameTimer` in `Game`, `connect` timeout -> update `TimerLabel`.
7. Win/Lose: Zeige `GameOverDialog`, deaktiviere Board.
8. Persistence: `HighscoreManager` speichert JSON in `QStandardPaths::AppLocalDataLocation`.

**Verifikation / Tests**
1. Unit-Tests für `Board::placeMines`, `countAdjacent`, `revealCell` (falls Test-Framework vorhanden).
2. Manuelle Tests: 1) Erste-Klick-Safe, 2) Flood-fill auf leeren Bereich, 3) Flaggen verhindern Aufdecken, 4) Gewinnbedingung alle nicht-Minen aufgedeckt, 5) Verlust bei Aufdeckung Mine.
3. UI-Tests: Skalierungstest bei verschiedenen window sizes, usability: right-click flag funktioniert auf Touchpads?

**Aufteilen in implementierbare Tasks (für Team / Einzelarbeit)**
Phase A — Setup & Model (parallel mit Phase B UI skeleton)
- A1: `cell.h`, `board.h/.cpp` (2–4h)
- A2: `game.h/.cpp` (2–4h)
Phase B — UI Skeleton
- B1: `mainwindow.ui`: Steuerleiste + Platzhalter `boardContainer` (0.5–1h)
- B2: `cellwidget.h/.cpp`, `boardwidget.h/.cpp` (2–4h)
Phase C — Integration & Logic
- C1: Connect signals/slots, implement reveal/flag flow (2–4h)
- C2: Timer, mines counter, status messages (1–2h)
Phase D — Polish & Extras
- D1: Highscore/Settings dialogs, icons, styles (2–4h)
- D2: Tests, edge-case fixes, README (1–2h)

**Decisions & Annahmen**
- Annahme: Standardspielregeln von Minesweeper gelten (Erster Klick darf keine Mine auslösen).
- Annahme: Qt6 und CMake bereits korrekt konfiguriert (siehe vorhandene `CMakeLists.txt`).
- Ausgeschlossen: Netzwerk-Highscores oder Online-Multiplayer (Scope minimal halten).

**Weiteres / Fragen an dich**
1. Möchtest du Standardgrößen (Einfach: 9×9/10 Minen, Mittel: 16×16/40, Schwer: 16×30/99) oder eigene Werte aus `u03.pdf` übernehmen? 
2. Soll `CellWidget` als `QPushButton`-Subclass oder vollständig custom painted `QWidget` implementiert werden? Empfehlung: `QPushButton`-Subclass für einfache Click/Right-Click-Logik.

---
Wenn du willst, schreibe ich jetzt aus diesem Plan die konkrete To‑Do-Liste und die List der zu erstellenden Dateien, oder ich kann den Plan in einer echten `Plan.md` im Projektordner ablegen — sag mir, ob ich das hier ablegen darf.