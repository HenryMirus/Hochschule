#include "game.h"

#include "statistics.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QPair>
#include <QtCore/QRandomGenerator>
#include <QtCore/QTimer>
#include <QtCore/QStringList>

#include <QtCore/QtGlobal>

#include <initializer_list>

#include <queue>

namespace
{
constexpr int kDefaultRows = 9;
constexpr int kDefaultCols = 9;
constexpr int kDefaultMines = 10;
}

/*
 * Autor: Henry Mirus
 * Eingabe: width  – Anzahl der Spalten
 *          height – Anzahl der Zeilen
 *          mines  – Anzahl der Minen
 *          parent – übergeordnetes QObject
 * Rückgabe: –
 * Seiteneffekte: Verbindet den internen Timer und startet sofort ein Spiel
 *                mit den gespeicherten oder übergebenen Einstellungen.
 */
Game::Game(int width, int height, int mines, QObject *parent)
    : QObject(parent)
    , m_cols(width)
    , m_rows(height)
    , m_mines(mines)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &Game::onTimerTimeout);
    restartFromSettings();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Einstellungen aus der settings.json im Programmverzeichnis;
 *           bei Fehler werden die Konstruktorwerte bzw. Standardwerte verwendet
 * Seiteneffekte: –
 */
Game::Settings Game::readSettingsFromBuildDirectory() const
{
    Settings settings;
    settings.rows = m_rows > 0 ? m_rows : kDefaultRows;
    settings.cols = m_cols > 0 ? m_cols : kDefaultCols;
    settings.mines = m_mines > 0 ? m_mines : kDefaultMines;

    const QString filePath = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("settings.json"));
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return settings;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject()) {
        return settings;
    }

    const QJsonObject object = document.object();
    const auto readInt = [&object](std::initializer_list<const char *> keys, int fallback) {
        for (const char *key : keys) {
            const QJsonValue value = object.value(QString::fromUtf8(key));
            if (value.isDouble()) {
                return value.toInt(fallback);
            }
        }
        return fallback;
    };

    settings.rows = qMax(1, readInt({"rows", "n", "height"}, settings.rows));
    settings.cols = qMax(1, readInt({"cols", "m", "width"}, settings.cols));
    const int maxMines = qMax(0, settings.rows * settings.cols - 1);
    settings.mines = qBound(0, readInt({"mines", "k"}, settings.mines), maxMines);

    const QJsonValue seedValue = object.value(QStringLiteral("seed"));
    if (seedValue.isDouble()) {
        settings.hasSeed = true;
        settings.seed = static_cast<quint32>(seedValue.toVariant().toULongLong());
    }

    return settings;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Liest die gespeicherten Einstellungen und startet ein neues
 *                Spiel mit diesen Parametern.
 */
void Game::restartFromSettings()
{
    clearBoard();
    const Settings settings = readSettingsFromBuildDirectory();
    startGame(settings.rows, settings.cols, settings.mines, settings.hasSeed, settings.seed);
}

/*
 * Autor: Henry Mirus
 * Eingabe: rows    – Anzahl der Zeilen
 *          cols    – Anzahl der Spalten
 *          mines   – Anzahl der Minen
 *          useSeed – true, wenn ein fester Zufallsseed verwendet werden soll
 *          seed    – Zufallsseed (wird nur bei useSeed == true ausgewertet)
 * Rückgabe: –
 * Seiteneffekte: Setzt das Spielfeld zurück, legt ein neues Board an und
 *                sendet boardCreated sowie gameStarted.
 */
void Game::startGame(int rows, int cols, int mines, bool useSeed, quint32 seed)
{
    clearBoard();

    m_lastSettings.rows = qMax(1, rows);
    m_lastSettings.cols = qMax(1, cols);
    const int maxMines = qMax(0, m_lastSettings.rows * m_lastSettings.cols - 1);
    m_lastSettings.mines = qBound(0, mines, maxMines);
    m_lastSettings.hasSeed = useSeed;
    m_lastSettings.seed = seed;

    createBoard(m_lastSettings.rows, m_lastSettings.cols, m_lastSettings.mines, useSeed, seed);
    updateStatus();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Stoppt den Timer, leert das Spielfeld, setzt alle
 *                Zustandsvariablen zurück und sendet boardCleared sowie
 *                timerChanged(0).
 */
void Game::clearBoard()
{
    if (m_timer != nullptr) {
        m_timer->stop();
    }

    m_board.clear();
    m_flagCount = 0;
    m_elapsedSeconds = 0;
    m_running = false;
    m_paused = false;
    m_gameOver = false;
    m_minesPlaced = false;
    m_useSeed = false;
    m_seed = 0;

    emit boardCleared();
    emit timerChanged(0);
}

/*
 * Autor: Henry Mirus
 * Eingabe: rows    – Anzahl der Zeilen
 *          cols    – Anzahl der Spalten
 *          mines   – Anzahl der Minen
 *          useSeed – true, wenn ein fester Zufallsseed verwendet werden soll
 *          seed    – Zufallsseed
 * Rückgabe: –
 * Seiteneffekte: Initialisiert das leere Spielfeld (ohne Minen), setzt
 *                m_running auf true und sendet boardCreated sowie gameStarted.
 */
void Game::createBoard(int rows, int cols, int mines, bool useSeed, quint32 seed)
{
    m_rows = rows;
    m_cols = cols;
    m_mines = mines;
    m_useSeed = useSeed;
    m_seed = seed;

    m_board = QVector<QVector<Cell>>(rows, QVector<Cell>(cols));
    m_running = true;
    m_paused = false;
    m_gameOver = false;
    m_minesPlaced = false;
    m_flagCount = 0;
    m_elapsedSeconds = 0;

    emit boardCreated(rows, cols, mines);
    emit gameStarted();
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des aufzudeckenden Feldes
 *          col – Spaltenindex des aufzudeckenden Feldes
 * Rückgabe: –
 * Seiteneffekte: Deckt das Feld auf; bei erstem Aufruf werden Minen platziert
 *                und der Timer gestartet. Bei einer Mine wird finishGame(false)
 *                ausgelöst; bei leerem Feld wird Flood-Fill durchgeführt.
 *                Sendet cellChanged für betroffene Felder.
 */
void Game::revealCell(int row, int col)
{
    if (!isValidCell(row, col) || m_gameOver || m_paused) {
        return;
    }

    if (!m_timer->isActive()) {
        m_timer->start(1000);
    }

    if (!m_minesPlaced) {
        placeMines(row, col);
    }

    Cell &cell = m_board[row][col];
    if (cell.isFlagged || cell.isRevealed) {
        return;
    }

    if (cell.isMine) {
        cell.isRevealed = true;
        cell.isExploded = true;
        emit cellChanged(row, col);
        finishGame(false);
        return;
    }

    revealFloodFill(row, col);
    if (countHiddenCells() == 0) {
        finishGame(countCorrectlyFlaggedMines() == m_mines && m_flagCount == m_mines);
        return;
    }

    updateStatus();
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des zu markierenden Feldes
 *          col – Spaltenindex des zu markierenden Feldes
 * Rückgabe: –
 * Seiteneffekte: Setzt eine Flagge auf dem Feld (nur wenn noch keine gesetzt
 *                und das Spiel läuft). Erhöht m_flagCount, sendet cellChanged
 *                und prüft Spielende. Vor dem ersten Klick wirkungslos.
 */
void Game::setFlag(int row, int col)
{
    if (!isValidCell(row, col) || m_gameOver || m_paused || !m_minesPlaced) {
        return;
    }

    Cell &cell = m_board[row][col];
    if (cell.isRevealed || cell.isFlagged) {
        return;
    }

    cell.isFlagged = true;
    ++m_flagCount;
    emit cellChanged(row, col);

    if (countHiddenCells() == 0) {
        finishGame(countCorrectlyFlaggedMines() == m_mines && m_flagCount == m_mines);
        return;
    }

    updateStatus();
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des Feldes
 *          col – Spaltenindex des Feldes
 * Rückgabe: –
 * Seiteneffekte: Schaltet den Flaggenzustand um; passt m_flagCount an,
 *                sendet cellChanged und prüft Spielende.
 *                Vor dem ersten Klick wirkungslos.
 */
void Game::toggleFlag(int row, int col)
{
    if (!isValidCell(row, col) || m_gameOver || m_paused || !m_minesPlaced) {
        return;
    }

    Cell &cell = m_board[row][col];
    if (cell.isRevealed) {
        return;
    }

    cell.isFlagged = !cell.isFlagged;
    m_flagCount += cell.isFlagged ? 1 : -1;
    emit cellChanged(row, col);

    if (countHiddenCells() == 0) {
        finishGame(countCorrectlyFlaggedMines() == m_mines && m_flagCount == m_mines);
        return;
    }

    updateStatus();
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des Feldes
 *          col – Spaltenindex des Feldes
 * Rückgabe: –
 * Seiteneffekte: Entfernt die Flagge vom Feld (nur wenn eine gesetzt ist).
 *                Verringert m_flagCount und sendet cellChanged.
 *                Vor dem ersten Klick wirkungslos.
 */
void Game::removeFlag(int row, int col)
{
    if (!isValidCell(row, col) || m_gameOver || m_paused || !m_minesPlaced) {
        return;
    }

    Cell &cell = m_board[row][col];
    if (!cell.isFlagged) {
        return;
    }

    cell.isFlagged = false;
    m_flagCount = qMax(0, m_flagCount - 1);
    emit cellChanged(row, col);
    updateStatus();
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des Feldes
 *          col – Spaltenindex des Feldes
 * Rückgabe: –
 * Seiteneffekte: Entfernt die Flagge des Feldes (mittlerer Mausklick-Aktion).
 */
void Game::middleClick(int row, int col)
{
    removeFlag(row, col);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Pausiert ein laufendes Spiel: Timer wird gestoppt,
 *                m_paused auf true gesetzt, pausedChanged(true) gesendet.
 *                Vor dem ersten Klick wirkungslos.
 */
void Game::pauseGame()
{
    if (m_gameOver || !m_running || m_paused || !m_minesPlaced) {
        return;
    }

    m_paused = true;
    m_timer->stop();
    emit pausedChanged(true);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Setzt ein pausiertes Spiel fort: Timer wird wieder gestartet,
 *                m_paused auf false gesetzt, pausedChanged(false) gesendet.
 */
void Game::resumeGame()
{
    if (m_gameOver || !m_running || !m_paused) {
        return;
    }

    m_paused = false;
    m_timer->start(1000);
    emit pausedChanged(false);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Deckt alle Minenfelder auf, ruft finishGame(false) auf und
 *                zählt das Spiel als verloren. Vor dem ersten Klick wirkungslos.
 */
void Game::endGame()
{
    if (m_gameOver || !m_minesPlaced) {
        return;
    }

    for (int row = 0; row < m_board.size(); ++row) {
        for (int col = 0; col < m_board[row].size(); ++col) {
            Cell &cell = m_board[row][col];
            if (cell.isMine) {
                cell.isRevealed = true;
            }
            emit cellChanged(row, col);
        }
    }

    finishGame(false);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Speichert das Spielergebnis (Sieg/Niederlage und Zeit)
 *                in der Statistikdatei. Darf nur nach Spielende aufgerufen werden.
 */
void Game::saveGame()
{
    if (!m_gameOver) {
        return;
    }

    Statistics::saveGame(m_lastGameWon, m_elapsedSeconds);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzahl der Zeilen des Spielfelds
 * Seiteneffekte: –
 */
int Game::rows() const
{
    return m_rows;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzahl der Spalten des Spielfelds
 * Seiteneffekte: –
 */
int Game::cols() const
{
    return m_cols;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Gesamtanzahl der Minen im Spielfeld
 * Seiteneffekte: –
 */
int Game::mineCount() const
{
    return m_mines;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzahl der aktuell gesetzten Flaggen
 * Seiteneffekte: –
 */
int Game::flagCount() const
{
    return m_flagCount;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Vergangene Spielzeit in Sekunden seit dem ersten Klick
 * Seiteneffekte: –
 */
int Game::elapsedSeconds() const
{
    return m_elapsedSeconds;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: true, wenn das Spiel aktiv läuft (weder beendet noch pausiert)
 * Seiteneffekte: –
 */
bool Game::isRunning() const
{
    return m_running;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: true, wenn das Spiel pausiert ist
 * Seiteneffekte: –
 */
bool Game::isPaused() const
{
    return m_paused;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: true, wenn das Spiel beendet ist (gewonnen oder verloren)
 * Seiteneffekte: –
 */
bool Game::isGameOver() const
{
    return m_gameOver;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: true, wenn Minen bereits auf dem Spielfeld platziert wurden
 *           (d. h. der erste Klick erfolgt ist)
 * Seiteneffekte: –
 */
bool Game::hasMinesPlaced() const
{
    return m_minesPlaced;
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex
 *          col – Spaltenindex
 * Rückgabe: true, wenn (row, col) innerhalb der Spielfeldgrenzen liegt
 * Seiteneffekte: –
 */
bool Game::isValidCell(int row, int col) const
{
    return row >= 0 && col >= 0 && row < m_board.size() && (!m_board.isEmpty() ? col < m_board.first().size() : false);
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des Feldes
 *          col – Spaltenindex des Feldes
 * Rückgabe: Anzeigestatus des Feldes als CellDisplayState-Enum-Wert;
 *           berücksichtigt den Spielendzustand und alle Unterzustände
 * Seiteneffekte: –
 */
Game::CellDisplayState Game::cellDisplayState(int row, int col) const
{
    if (!isValidCell(row, col)) {
        return CellDisplayState::Hidden;
    }

    const Cell &cell = m_board[row][col];
    if (!m_gameOver) {
        if (cell.isFlagged) {
            return CellDisplayState::Flagged;
        }
        if (cell.isExploded) {
            return CellDisplayState::ExplodedMine;
        }
        if (cell.isRevealed && cell.isMine) {
            return CellDisplayState::RevealedMine;
        }
        if (cell.isRevealed && cell.adjacentMines > 0) {
            return CellDisplayState::RevealedNumber;
        }
        if (cell.isRevealed) {
            return CellDisplayState::RevealedEmpty;
        }
        return CellDisplayState::Hidden;
    }

    if (cell.isFlagged && cell.isMine) {
        return CellDisplayState::CorrectFlag;
    }
    if (cell.isFlagged && !cell.isMine) {
        return CellDisplayState::WrongFlag;
    }
    if (cell.isExploded) {
        return CellDisplayState::ExplodedMine;
    }
    if (cell.isMine) {
        return CellDisplayState::RevealedMine;
    }
    if (cell.adjacentMines > 0) {
        return CellDisplayState::RevealedNumber;
    }
    return CellDisplayState::RevealedEmpty;
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des Feldes
 *          col – Spaltenindex des Feldes
 * Rückgabe: Anzahl der verminten Nachbarfelder; 0 bei ungültigem Index
 * Seiteneffekte: –
 */
int Game::adjacentMines(int row, int col) const
{
    if (!isValidCell(row, col)) {
        return 0;
    }

    return m_board[row][col].adjacentMines;
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des Feldes
 *          col – Spaltenindex des Feldes
 * Rückgabe: Konstante Referenz auf das Cell-Objekt an der angegebenen Position
 * Seiteneffekte: –
 */
const Game::Cell &Game::cellAt(int row, int col) const
{
    return m_board[row][col];
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Erhöht m_elapsedSeconds um 1 und sendet timerChanged
 *                sowie updateStatus. Wird einmal pro Sekunde durch den Timer ausgelöst.
 */
void Game::onTimerTimeout()
{
    if (m_gameOver || m_paused || !m_running) {
        return;
    }

    ++m_elapsedSeconds;
    emit timerChanged(m_elapsedSeconds);
    updateStatus();
}

/*
 * Autor: Henry Mirus
 * Eingabe: firstRow – Zeile des ersten aufgedeckten Feldes (Mine-freie Zone)
 *          firstCol – Spalte des ersten aufgedeckten Feldes (Mine-freie Zone)
 * Rückgabe: –
 * Seiteneffekte: Verteilt Minen zufällig auf das Spielfeld (Schutzzone um
 *                den ersten Klick bleibt minenfrei), berechnet adjacentMines
 *                für alle Felder, setzt m_minesPlaced auf true und sendet
 *                firstCellRevealed.
 */
void Game::placeMines(int firstRow, int firstCol)
{
    if (m_minesPlaced || m_board.isEmpty()) {
        return;
    }

    QVector<QPair<int, int>> candidates;
    candidates.reserve(m_rows * m_cols);

    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            if (isInsideSafeZone(row, col, firstRow, firstCol)) {
                continue;
            }
            candidates.append(qMakePair(row, col));
        }
    }

    if (candidates.size() < m_mines) {
        candidates.clear();
        for (int row = 0; row < m_rows; ++row) {
            for (int col = 0; col < m_cols; ++col) {
                if (row == firstRow && col == firstCol) {
                    continue;
                }
                candidates.append(qMakePair(row, col));
            }
        }
    }

    QRandomGenerator generator = m_useSeed ? QRandomGenerator(m_seed) : QRandomGenerator::securelySeeded();
    for (int index = candidates.size() - 1; index > 0; --index) {
        const int swapIndex = generator.bounded(index + 1);
        qSwap(candidates[index], candidates[swapIndex]);
    }

    for (int mineIndex = 0; mineIndex < m_mines && mineIndex < candidates.size(); ++mineIndex) {
        const QPair<int, int> position = candidates[mineIndex];
        m_board[position.first][position.second].isMine = true;
    }

    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            m_board[row][col].adjacentMines = countAdjacentMines(row, col);
        }
    }

    m_minesPlaced = true;
    emit firstCellRevealed();
}

/*
 * Autor: Henry Mirus
 * Eingabe: startRow – Zeile des Startfeldes für den Flood-Fill
 *          startCol – Spalte des Startfeldes für den Flood-Fill
 * Rückgabe: –
 * Seiteneffekte: Deckt iterativ alle zusammenhängenden leeren Felder auf
 *                und sendet cellChanged für jedes aufgedeckte Feld.
 */
void Game::revealFloodFill(int startRow, int startCol)
{
    if (!isValidCell(startRow, startCol)) {
        return;
    }

    std::queue<QPair<int, int>> pending;
    pending.emplace(startRow, startCol);

    while (!pending.empty()) {
        const QPair<int, int> current = pending.front();
        pending.pop();

        const int row = current.first;
        const int col = current.second;
        if (!isValidCell(row, col)) {
            continue;
        }

        Cell &cell = m_board[row][col];
        if (cell.isRevealed || cell.isFlagged || cell.isMine) {
            continue;
        }

        cell.isRevealed = true;
        emit cellChanged(row, col);

        if (cell.adjacentMines != 0) {
            continue;
        }

        for (int deltaRow = -1; deltaRow <= 1; ++deltaRow) {
            for (int deltaCol = -1; deltaCol <= 1; ++deltaCol) {
                if (deltaRow == 0 && deltaCol == 0) {
                    continue;
                }

                const int neighborRow = row + deltaRow;
                const int neighborCol = col + deltaCol;
                if (isValidCell(neighborRow, neighborCol)) {
                    pending.emplace(neighborRow, neighborCol);
                }
            }
        }
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: row – Zeilenindex des Feldes
 *          col – Spaltenindex des Feldes
 * Rückgabe: Anzahl der verminten Felder in der direkten Nachbarschaft (0–8)
 * Seiteneffekte: –
 */
int Game::countAdjacentMines(int row, int col) const
{
    int count = 0;
    for (int deltaRow = -1; deltaRow <= 1; ++deltaRow) {
        for (int deltaCol = -1; deltaCol <= 1; ++deltaCol) {
            if (deltaRow == 0 && deltaCol == 0) {
                continue;
            }

            const int neighborRow = row + deltaRow;
            const int neighborCol = col + deltaCol;
            if (isValidCell(neighborRow, neighborCol) && m_board[neighborRow][neighborCol].isMine) {
                ++count;
            }
        }
    }
    return count;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzahl der Felder, die weder aufgedeckt noch markiert sind
 * Seiteneffekte: –
 */
int Game::countHiddenCells() const
{
    int hiddenCount = 0;
    for (const QVector<Cell> &row : m_board) {
        for (const Cell &cell : row) {
            if (!cell.isRevealed && !cell.isFlagged) {
                ++hiddenCount;
            }
        }
    }
    return hiddenCount;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Anzahl der Minen, die korrekt mit einer Flagge markiert sind
 * Seiteneffekte: –
 */
int Game::countCorrectlyFlaggedMines() const
{
    int correctlyFlagged = 0;
    for (const QVector<Cell> &row : m_board) {
        for (const Cell &cell : row) {
            if (cell.isMine && cell.isFlagged) {
                ++correctlyFlagged;
            }
        }
    }
    return correctlyFlagged;
}

/*
 * Autor: Henry Mirus
 * Eingabe: won – true, wenn das Spiel gewonnen wurde
 * Rückgabe: –
 * Seiteneffekte: Setzt m_gameOver auf true, stoppt den Timer, deckt alle
 *                verbleibenden Minen auf, sendet cellChanged für alle Felder,
 *                dann gameFinished(won) und speichert das Ergebnis in der
 *                Statistik.
 */
void Game::finishGame(bool won)
{
    if (m_gameOver) {
        return;
    }

    m_gameOver = true;
    m_lastGameWon = won;
    m_running = false;
    m_paused = false;
    if (m_timer != nullptr) {
        m_timer->stop();
    }

    for (int row = 0; row < m_board.size(); ++row) {
        for (int col = 0; col < m_board[row].size(); ++col) {
            Cell &cell = m_board[row][col];
            if (cell.isMine && !cell.isFlagged) {
                cell.isRevealed = true;
            }
            emit cellChanged(row, col);
        }
    }

    updateStatus();
    emit gameFinished(won);
    saveGame();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Sendet statusChanged mit den aktuellen Werten für Minen,
 *                Flaggen und vergangene Zeit.
 */
void Game::updateStatus()
{
    emit statusChanged(m_mines, m_flagCount, m_elapsedSeconds);
}

/*
 * Autor: Henry Mirus
 * Eingabe: row     – zu prüfende Zeile
 *          col     – zu prüfende Spalte
 *          safeRow – Zeile des ersten Klicks
 *          safeCol – Spalte des ersten Klicks
 * Rückgabe: true, wenn (row, col) innerhalb des 3×3-Schutzbereichs um
 *           (safeRow, safeCol) liegt
 * Seiteneffekte: –
 */
bool Game::isInsideSafeZone(int row, int col, int safeRow, int safeCol) const
{
    return qAbs(row - safeRow) <= 1 && qAbs(col - safeCol) <= 1;
}
