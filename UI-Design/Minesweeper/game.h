#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QString>
#include <QVector>

class QTimer;

/*
 * Autor: Henry Mirus
 * Zweck: Verwaltet den gesamten Spielzustand eines Minesweeper-Spiels.
 *        Dazu gehören das Spielfeld mit allen Zellen und deren Zuständen,
 *        die Minenverteilung, das Setzen und Entfernen von Flaggen, der
 *        Spieltimer sowie die Erkennung von Sieg und Niederlage.
 */
class Game : public QObject
{
	Q_OBJECT

public:
	enum class CellDisplayState
	{
		Hidden,
		Flagged,
		RevealedEmpty,
		RevealedNumber,
		RevealedMine,
		ExplodedMine,
		CorrectFlag,
		WrongFlag
	};

	struct Cell
	{
		bool isMine = false;
		bool isRevealed = false;
		bool isFlagged = false;
		bool isExploded = false;
		int adjacentMines = 0;
	};

	struct Settings
	{
		int rows = 9;
		int cols = 9;
		int mines = 10;
		bool hasSeed = false;
		quint32 seed = 0;
	};

	explicit Game(int width, int height, int mines, QObject *parent = nullptr);

	void restartFromSettings();
	void startGame(int rows, int cols, int mines, bool useSeed = false, quint32 seed = 0);
	void revealCell(int row, int col);
	void setFlag(int row, int col);
	void toggleFlag(int row, int col);
	void removeFlag(int row, int col);
	void middleClick(int row, int col);
	void pauseGame();
	void resumeGame();
	void endGame();
	void saveGame();

	int rows() const;
	int cols() const;
	int mineCount() const;
	int flagCount() const;
	int elapsedSeconds() const;
	bool isRunning() const;
	bool isPaused() const;
	bool isGameOver() const;
	bool hasMinesPlaced() const;

	bool isValidCell(int row, int col) const;
	CellDisplayState cellDisplayState(int row, int col) const;
	int adjacentMines(int row, int col) const;
	const Cell &cellAt(int row, int col) const;

signals:
	void boardCleared();
	void boardCreated(int rows, int cols, int mines);
	void cellChanged(int row, int col);
	void statusChanged(int mines, int flags, int elapsedSeconds);
	void timerChanged(int elapsedSeconds);
	void gameFinished(bool won);
	void gameStarted();
	void firstCellRevealed();
	void pausedChanged(bool paused);

private slots:
	void onTimerTimeout();

private:
	Settings readSettingsFromBuildDirectory() const;
	void clearBoard();
	void createBoard(int rows, int cols, int mines, bool useSeed, quint32 seed);
	void placeMines(int firstRow, int firstCol);
	void revealFloodFill(int startRow, int startCol);
	int countAdjacentMines(int row, int col) const;
	int countHiddenCells() const;
	int countCorrectlyFlaggedMines() const;
	void finishGame(bool won);
	void updateStatus();
	bool isInsideSafeZone(int row, int col, int safeRow, int safeCol) const;

	QVector<QVector<Cell>> m_board;
	int m_cols = 0;
	int m_rows = 0;
	int m_mines = 0;
	int m_flagCount = 0;
	int m_elapsedSeconds = 0;
	bool m_running = false;
	bool m_paused = false;
	bool m_gameOver = false;
	bool m_minesPlaced = false;
	bool m_useSeed = false;
	quint32 m_seed = 0;
	bool m_lastGameWon = false;
	Settings m_lastSettings;
	QTimer *m_timer = nullptr;
};

#endif // GAME_H
