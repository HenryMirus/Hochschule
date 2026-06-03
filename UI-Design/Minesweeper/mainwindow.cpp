#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "helpdialog.h"
#include "cellbutton.h"
#include "settingsdialog.h"
#include "statisticdialog.h"

#include <QAction>
#include <QCloseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QLayoutItem>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QtGlobal>

namespace
{
constexpr int kMaxCellSize = 28;
}

/*
 * Autor: Henry Mirus
 * Eingabe: parent – übergeordnetes Widget
 * Rückgabe: –
 * Seiteneffekte: Lädt Einstellungen, erzeugt das Game-Objekt, verbindet alle
 *                Signale und Slots, baut das Spielfeld auf und zeigt die
 *                Statuszeile an.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_game(nullptr)
    , m_minesLabel(nullptr)
    , m_flagsLabel(nullptr)
    , m_timeLabel(nullptr)
    , m_settingsDialog(nullptr)
    , m_statisticsDialog(nullptr)
    , m_helpDialog(nullptr)
    , m_isPaused(false)
    , m_playerEndedGame(false)
{
    ui->setupUi(this);

    m_settings = Settings::loadSettings();
    if (!m_settings.isValid()) {
        m_settings.rows = 9;
        m_settings.columns = 9;
        m_settings.mines = 10;
        Settings::saveSettings(m_settings);
    }

    m_game = new Game(m_settings.columns, m_settings.rows, m_settings.mines, this);

    m_minesLabel = new QLabel(this);
    m_flagsLabel = new QLabel(this);
    m_timeLabel = new QLabel(this);

    ui->statusbar->addPermanentWidget(m_minesLabel);
    ui->statusbar->addPermanentWidget(m_flagsLabel);
    ui->statusbar->addPermanentWidget(m_timeLabel);

    connect(ui->actionEinstellungen, &QAction::triggered, this, &MainWindow::openSettingsDialog);
    connect(ui->actionStatistik, &QAction::triggered, this, &MainWindow::openStatisticsDialog);
    connect(ui->actionHilfe, &QAction::triggered, this, &MainWindow::openHelpDialog);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::startNewGame);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::togglePause);
    connect(ui->pushButton_3, &QPushButton::clicked, this, [this]() {
        m_playerEndedGame = true;
        m_game->endGame();
    });

    connect(m_game, &Game::statusChanged, this, &MainWindow::updateStatusBar);
    connect(m_game, &Game::boardCreated, this, [this](int rows, int columns, int) {
        m_isPaused = false;
        buildBoard(rows, columns);
    });
    connect(m_game, &Game::boardCleared, this, &MainWindow::clearBoard);
    connect(m_game, &Game::cellChanged, this, &MainWindow::updateCell);
    connect(m_game, &Game::firstCellRevealed, this, [this]() {
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
    });
    connect(m_game, &Game::gameFinished, this, [this](bool won) {
        syncBoardFromGame();
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);

        if (won) {
            QMessageBox::information(this, QStringLiteral("Gewonnen!"),
                QStringLiteral("Glückwunsch! Sie haben alle Minen gefunden."));
        } else if (!m_playerEndedGame) {
            bool mineExploded = false;
            for (int r = 0; r < m_game->rows() && !mineExploded; ++r) {
                for (int c = 0; c < m_game->cols() && !mineExploded; ++c) {
                    if (m_game->cellAt(r, c).isExploded)
                        mineExploded = true;
                }
            }
            if (mineExploded) {
                QMessageBox::information(this, QStringLiteral("Verloren!"),
                    QStringLiteral("Sie haben eine Mine getroffen. Viel Erfolg beim nächsten Mal!"));
            } else {
                QMessageBox::information(this, QStringLiteral("Verloren!"),
                    QStringLiteral("Zu viele oder falsche Markierungen gesetzt. Viel Erfolg beim nächsten Mal!"));
            }
        }
        m_playerEndedGame = false;
    });

    startNewGame();
    updateStatusBar(m_settings.mines, 0, 0);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Gibt das UI-Objekt frei.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * Autor: Henry Mirus
 * Eingabe: event – Schließereignis des Fensters
 * Rückgabe: –
 * Seiteneffekte: Speichert die aktuellen Einstellungen in der JSON-Datei
 *                und gibt das Ereignis an die Basisklasse weiter.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!Settings::saveSettings(m_settings)) {
        qWarning() << "Could not save settings";
    }

    QMainWindow::closeEvent(event);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Öffnet den Einstellungsdialog; bei Bestätigung werden die
 *                neuen Einstellungen gespeichert und ein neues Spiel gestartet.
 */
void MainWindow::openSettingsDialog()
{
    if (m_settingsDialog == nullptr) {
        m_settingsDialog = new SettingsDialog(this);
    }

    m_settingsDialog->setSettings(m_settings);
    if (m_settingsDialog->exec() == QDialog::Accepted) {
        m_settings = m_settingsDialog->settings();
        Settings::saveSettings(m_settings);
        startNewGame();
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Öffnet den Statistikdialog und lädt die aktuellen
 *                Statistikdaten neu.
 */
void MainWindow::openStatisticsDialog()
{
    if (m_statisticsDialog == nullptr) {
        m_statisticsDialog = new StatisticDialog(this);
    }

    m_statisticsDialog->reloadStatistics();
    m_statisticsDialog->exec();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Öffnet den Hilfedialog mit der Spielanleitung.
 */
void MainWindow::openHelpDialog()
{
    if (m_helpDialog == nullptr) {
        m_helpDialog = new HelpDialog(this);
    }

    m_helpDialog->exec();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Setzt den Pausenzustand zurück, deaktiviert Pause- und
 *                Beenden-Schalter und startet ein neues Spiel mit den
 *                gespeicherten Einstellungen.
 */
void MainWindow::startNewGame()
{
    m_isPaused = false;
    ui->pushButton_2->setText(QStringLiteral("Pause"));
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    m_game->startGame(m_settings.rows, m_settings.columns, m_settings.mines);
    updateStatusBar(m_settings.mines, 0, 0);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Wechselt zwischen Pause und Weiterlaufen. Im Pausezustand
 *                werden alle Felder ausgeblendet; beim Fortsetzen wird das
 *                Spielfeld wiederhergestellt.
 */
void MainWindow::togglePause()
{
    if (m_isPaused) {
        m_game->resumeGame();
        ui->pushButton_2->setText(QStringLiteral("Pause"));
        m_isPaused = false;
        syncBoardFromGame();
    } else {
        m_game->pauseGame();
        ui->pushButton_2->setText(QStringLiteral("Weiter"));
        m_isPaused = true;
        const QString pausedStyle = QStringLiteral(
            "background-color: palette(window); border: 1px solid palette(window); padding: 0px;");
        for (auto &row : m_cellButtons) {
            for (auto *btn : row) {
                btn->setEnabled(false);
                btn->setText(QString());
                btn->setStyleSheet(pausedStyle);
            }
        }
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: mines          – Gesamtanzahl der Minen
 *          flags          – Anzahl der gesetzten Flaggen
 *          elapsedSeconds – Vergangene Spielzeit in Sekunden
 * Rückgabe: –
 * Seiteneffekte: Aktualisiert die drei Labels in der Statuszeile;
 *                Flaggenanzahl wird rot dargestellt, wenn sie die
 *                Minenanzahl übersteigt.
 */
void MainWindow::updateStatusBar(int mines, int flags, int elapsedSeconds)
{
    m_minesLabel->setText(QStringLiteral("Minen: %1").arg(mines));
    m_timeLabel->setText(QStringLiteral("Zeit: %1 s").arg(elapsedSeconds));

    const bool tooManyFlags = flags > mines;
    const QString flagColor = tooManyFlags ? QStringLiteral("#e74c3c") : QStringLiteral("#64B5F6");
    m_flagsLabel->setText(QStringLiteral("Marken: <span style='color: %1; font-weight: bold;'>%2</span>")
                          .arg(flagColor, QString::number(flags)));
}

/*
 * Autor: Henry Mirus
 * Eingabe: rows    – Anzahl der Zeilen des neuen Spielfelds
 *          columns – Anzahl der Spalten des neuen Spielfelds
 * Rückgabe: –
 * Seiteneffekte: Löscht das bestehende Gitter, erstellt neue CellButton-
 *                Instanzen, verbindet deren Signale mit der Game-Logik und
 *                passt die Fenstergröße an.
 */
void MainWindow::buildBoard(int rows, int columns)
{
    clearBoard();

    ui->gridLayout->setSpacing(1);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);

    m_cellButtons.resize(rows);
    for (int row = 0; row < rows; ++row) {
        m_cellButtons[row].resize(columns);
        for (int column = 0; column < columns; ++column) {
            auto *button = new CellButton(row, column, this);
            button->setProperty("mineCell", true);
            button->setFixedSize(kMaxCellSize, kMaxCellSize);
            ui->gridLayout->addWidget(button, row, column);
            m_cellButtons[row][column] = button;

            connect(button, &CellButton::leftClicked, this, [this](int r, int c) {
                m_game->revealCell(r, c);
            });
            connect(button, &CellButton::rightClicked, this, [this](int r, int c) {
                m_game->setFlag(r, c);
            });
            connect(button, &CellButton::middleClicked, this, [this](int r, int c) {
                m_game->removeFlag(r, c);
            });
        }
    }

    // Absorb all extra space after the real cells so they pack tightly together.
    // Reset any stretch from a previous game (different grid size).
    for (int r = 0; r <= 30; ++r) ui->gridLayout->setRowStretch(r, 0);
    for (int c = 0; c <= 30; ++c) ui->gridLayout->setColumnStretch(c, 0);
    ui->gridLayout->setRowStretch(rows, 1);
    ui->gridLayout->setColumnStretch(columns, 1);

    syncBoardFromGame();
    fitWindowToBoard(rows, columns);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Entfernt alle CellButton-Widgets aus dem Grid-Layout und
 *                leert m_cellButtons.
 */
void MainWindow::clearBoard()
{
    while (QLayoutItem *item = ui->gridLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    m_cellButtons.clear();
}

/*
 * Autor: Henry Mirus
 * Eingabe: row    – Zeilenindex des zu aktualisierenden Feldes
 *          column – Spaltenindex des zu aktualisierenden Feldes
 * Rückgabe: –
 * Seiteneffekte: Liest den Anzeigestatus des Feldes aus der Game-Instanz
 *                und aktualisiert Text und Stil des entsprechenden CellButton.
 */
void MainWindow::updateCell(int row, int column)
{
    if (row < 0 || column < 0 || row >= m_cellButtons.size()) {
        return;
    }
    if (column >= m_cellButtons[row].size()) {
        return;
    }

    CellButton *button = m_cellButtons[row][column];
    if (button == nullptr) {
        return;
    }

    const Game::CellDisplayState state = m_game->cellDisplayState(row, column);
    const Game::Cell &cell = m_game->cellAt(row, column);

    button->setEnabled(!m_game->isGameOver());
    button->setText(QString());

    switch (state) {
    case Game::CellDisplayState::Hidden:
        button->setStyleSheet(QStringLiteral("background-color: #4d4d4d; border: 1px solid #2b2b2b; color: white; font-weight: 600; padding: 0px;"));
        break;
    case Game::CellDisplayState::Flagged:
        button->setText(QStringLiteral("F"));
        button->setStyleSheet(QStringLiteral("background-color: #d4b000; border: 1px solid #6b5a00; color: black; font-weight: 700; padding: 0px;"));
        break;
    case Game::CellDisplayState::RevealedEmpty:
        button->setText(QString());
        button->setStyleSheet(QStringLiteral("background-color: #f7f7f7; border: 1px solid #bcbcbc; color: black; padding: 0px;"));
        break;
    case Game::CellDisplayState::RevealedNumber:
        button->setText(QString::number(cell.adjacentMines));
        button->setStyleSheet(QStringLiteral("background-color: #f7f7f7; border: 1px solid #bcbcbc; color: black; font-weight: 700; padding: 0px;"));
        break;
    case Game::CellDisplayState::RevealedMine:
        button->setText(QStringLiteral("X"));
        button->setStyleSheet(QStringLiteral("background-color: #f7f7f7; border: 1px solid #bcbcbc; color: black; font-weight: 700; padding: 0px;"));
        break;
    case Game::CellDisplayState::ExplodedMine:
        button->setText(QStringLiteral("X"));
        button->setStyleSheet(QStringLiteral("background-color: #f7f7f7; border: 2px solid #e74c3c; color: #e74c3c; font-weight: 700; padding: 0px;"));
        break;
    case Game::CellDisplayState::CorrectFlag:
        button->setText(QStringLiteral("F"));
        button->setStyleSheet(QStringLiteral("background-color: #3498db; border: 1px solid #2176ae; color: white; font-weight: 700; padding: 0px;"));
        break;
    case Game::CellDisplayState::WrongFlag:
        button->setText(QStringLiteral("F"));
        button->setStyleSheet(QStringLiteral("background-color: #e74c3c; border: 1px solid #b03a2e; color: white; font-weight: 700; padding: 0px;"));
        break;
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Ruft updateCell für alle Felder des Spielfelds auf, um
 *                die Darstellung vollständig mit dem Spielzustand zu synchronisieren.
 */
void MainWindow::syncBoardFromGame()
{
    if (m_cellButtons.isEmpty()) {
        return;
    }

    for (int row = 0; row < m_cellButtons.size(); ++row) {
        for (int column = 0; column < m_cellButtons[row].size(); ++column) {
            updateCell(row, column);
        }
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: rows    – Anzahl der Zeilen des aktuellen Spielfelds
 *          columns – Anzahl der Spalten des aktuellen Spielfelds
 * Rückgabe: –
 * Seiteneffekte: Berechnet die benötigte Fenstergröße aus Zellgröße, Abstands-
 *                und Rahmenmaßen und setzt das Fenster auf eine feste Größe.
 */
void MainWindow::fitWindowToBoard(int rows, int columns)
{
    const int spacing = ui->gridLayout->spacing();
    const int gridWidth = columns * kMaxCellSize + (columns - 1) * spacing;
    const int gridHeight = rows * kMaxCellSize + (rows - 1) * spacing;

    const QMargins cm = centralWidget()->layout()->contentsMargins();
    const int ls = centralWidget()->layout()->spacing();
    const int layoutSpacing = ls >= 0 ? ls : 6;
    const int hChrome = cm.left() + cm.right();
    const int vChrome = menuBar()->sizeHint().height()
                      + statusBar()->sizeHint().height()
                      + ui->pushButton->sizeHint().height()
                      + layoutSpacing + cm.top() + cm.bottom();

    setFixedSize(gridWidth + hChrome, gridHeight + vChrome);
}
