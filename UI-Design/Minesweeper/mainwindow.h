#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

#include "game.h"
#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QCloseEvent;
class QLabel;
class Game;
class CellButton;
class HelpDialog;
class SettingsDialog;
class StatisticDialog;

/*
 * Autor: Henry Mirus
 * Zweck: Hauptfenster der Minesweeper-Anwendung. Zeigt das Spielfeld als
 *        Gitter aus CellButton-Schaltflächen an, verbindet Benutzereingaben
 *        mit der Game-Logik und aktualisiert die Statuszeile sowie die
 *        Schaltflächen-Zustände (Neu, Pause, Beenden).
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openSettingsDialog();
    void openStatisticsDialog();
    void openHelpDialog();
    void startNewGame();
    void togglePause();
    void updateStatusBar(int mines, int flags, int elapsedSeconds);
    void buildBoard(int rows, int columns);
    void clearBoard();
    void updateCell(int row, int column);
    void syncBoardFromGame();
    void fitWindowToBoard(int rows, int columns);

private:
    Ui::MainWindow *ui;
    GameSettings m_settings;
    Game *m_game;
    QLabel *m_minesLabel;
    QLabel *m_flagsLabel;
    QLabel *m_timeLabel;
    SettingsDialog *m_settingsDialog;
    StatisticDialog *m_statisticsDialog;
    HelpDialog *m_helpDialog;
    bool m_isPaused;
    bool m_playerEndedGame;
    QVector<QVector<CellButton *>> m_cellButtons;
};
#endif // MAINWINDOW_H
