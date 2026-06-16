#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "einstellungsdialog.h"

#include <QAction>
#include <QLabel>
#include <QTimer>

/*
 * Autor: Henry Mirus
 * Eingabe: parent – übergeordnetes Widget
 * Rückgabe: –
 * Seiteneffekte: Initialisiert das UI (aus Qt-Designer), erzeugt drei
 *                Statusleisten-Labels sowie den Blink-Timer, verbindet alle
 *                Signale/Slots und startet das erste Spiel mit Standardwerten.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- Statuszeilen-Labels ---
    m_scheibenLabel = new QLabel(this);
    m_zugLabel      = new QLabel(this);
    m_zustandLabel  = new QLabel(this);

    // Feste Mindestbreiten verhindern Größensprünge beim Blinken
    m_scheibenLabel->setMinimumWidth(100);
    m_zugLabel->setMinimumWidth(120);
    m_zustandLabel->setMinimumWidth(90);

    ui->statusbar->addPermanentWidget(m_scheibenLabel);
    ui->statusbar->addPermanentWidget(m_zugLabel);
    ui->statusbar->addPermanentWidget(m_zustandLabel);

    // --- Blink-Timer für Pausen-Zustandsanzeige ---
    m_blinkTimer = new QTimer(this);
    m_blinkTimer->setInterval(500);
    connect(m_blinkTimer, &QTimer::timeout, this, &MainWindow::onBlinkTimer);

    // --- Schaltflächen-Slots ---
    connect(ui->neuButton,     &QPushButton::clicked,
            this, &MainWindow::onNeuGeklickt);
    connect(ui->startButton,   &QPushButton::clicked,
            ui->hanoiWidget,   &HanoiWidget::starteAnimation);
    connect(ui->pauseButton,   &QPushButton::clicked,
            ui->hanoiWidget,   &HanoiWidget::pauseWechsel);
    connect(ui->beendenButton, &QPushButton::clicked,
            ui->hanoiWidget, &HanoiWidget::beendeAnimation);
    connect(ui->einstellungenButton, &QPushButton::clicked,
            this, &MainWindow::openEinstellungsDialog);

    // --- Menü-Slots ---
    connect(ui->actionEinstellungen, &QAction::triggered,
            this, &MainWindow::openEinstellungsDialog);

    // --- HanoiWidget-Signale ---
    connect(ui->hanoiWidget, &HanoiWidget::zustandGeaendert,
            this, &MainWindow::onZustandGeaendert);
    connect(ui->hanoiWidget, &HanoiWidget::zugFortschrittGeaendert,
            this, &MainWindow::onZugFortschrittGeaendert);

    // --- Erstes Spiel starten ---
    // m_gesamtZuege wird über das Signal zugFortschrittGeaendert aktualisiert
    ui->hanoiWidget->neuesSpiel(m_einstellungen);
    adjustSize();
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
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Setzt lokale Zähler zurück (m_aktuellZugIndex = 0,
 *                m_gesamtZuege neu berechnet) und ruft hanoiWidget->neuesSpiel() auf.
 *                Das Widget sendet daraufhin zustandGeaendert(Bereit) und
 *                zugFortschrittGeaendert(0, …), die die Statuszeile aktualisieren.
 */
void MainWindow::onNeuGeklickt()
{
    m_aktuellZugIndex = 0;
    // m_gesamtZuege wird über das Signal zugFortschrittGeaendert aktualisiert
    ui->hanoiWidget->neuesSpiel(m_einstellungen);
    adjustSize();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Öffnet den EinstellungsDialog (erzeugt ihn beim ersten Aufruf).
 *                Bei Bestätigung (OK): Einstellungen übernehmen und neues Spiel starten.
 */
void MainWindow::openEinstellungsDialog()
{
    if (m_einstellungsDialog == nullptr) {
        m_einstellungsDialog = new EinstellungsDialog(this);
    }

    m_einstellungsDialog->setEinstellungen(m_einstellungen);
    if (m_einstellungsDialog->exec() == QDialog::Accepted) {
        m_einstellungen = m_einstellungsDialog->einstellungen();
        onNeuGeklickt();
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: zustand – neuer AnimationsZustand
 * Rückgabe: –
 * Seiteneffekte: Passt Schaltflächen-Text/-Aktivierung an:
 *                – Bereit:   Start aktiv,  Pause inaktiv
 *                – Laeuft:   Start inaktiv, Pause aktiv ("Pause")
 *                – Pausiert: Start inaktiv, Pause aktiv ("Weiter"), Blink-Timer an
 *                – Beendet:  Start inaktiv, Pause inaktiv
 *                Ruft updateStatusBar() auf.
 */
void MainWindow::onZustandGeaendert(AnimationsZustand zustand)
{
    m_aktuellZustand = zustand;
    m_blinkSichtbar  = true;
    m_blinkTimer->stop();

    switch (zustand) {
    case AnimationsZustand::Bereit:
        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->pauseButton->setText(QStringLiteral("Pause"));
        ui->beendenButton->setEnabled(false);
        break;

    case AnimationsZustand::Laeuft:
        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->pauseButton->setText(QStringLiteral("Pause"));
        ui->beendenButton->setEnabled(true);
        break;

    case AnimationsZustand::Pausiert:
        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->pauseButton->setText(QStringLiteral("Weiter"));
        ui->beendenButton->setEnabled(true);
        m_blinkTimer->start();
        break;

    case AnimationsZustand::Beendet:
        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(false);
        ui->pauseButton->setText(QStringLiteral("Pause"));
        ui->beendenButton->setEnabled(false);
        break;
    }

    updateStatusBar();
}

/*
 * Autor: Henry Mirus
 * Eingabe: fertigeZuege – abgeschlossene Züge
 *          gesamtZuege  – Gesamtzüge (wird ignoriert, da aus Einstellungen berechnet)
 * Rückgabe: –
 * Seiteneffekte: Aktualisiert m_aktuellZugIndex und ruft updateStatusBar() auf.
 */
void MainWindow::onZugFortschrittGeaendert(quint64 fertigeZuege, quint64 gesamtZuege)
{
    m_aktuellZugIndex = fertigeZuege;
    m_gesamtZuege     = gesamtZuege;
    updateStatusBar();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Schaltet m_blinkSichtbar um und aktualisiert den Zustandslabel
 *                im Pausenzustand (Blinken der Statuszeile).
 */
void MainWindow::onBlinkTimer()
{
    m_blinkSichtbar = !m_blinkSichtbar;
    updateStatusBar();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Schreibt aktuelle Werte in die drei Statusleisten-Labels:
 *                – m_scheibenLabel: Anzahl der Scheiben
 *                – m_zugLabel:      Zugfortschritt (X / Y)
 *                – m_zustandLabel:  Zustandstext (blinkt im Pausenzustand)
 */
void MainWindow::updateStatusBar()
{
    m_scheibenLabel->setText(
        QStringLiteral("Scheiben: %1").arg(m_einstellungen.scheibenzahl));

    m_zugLabel->setText(
        QStringLiteral("Zug: %1 / %2")
            .arg(QString::number(m_aktuellZugIndex))
            .arg(QString::number(m_gesamtZuege)));

    if (m_blinkSichtbar) {
        QString text;
        switch (m_aktuellZustand) {
        case AnimationsZustand::Bereit:   text = QStringLiteral("Startbereit"); break;
        case AnimationsZustand::Laeuft:   text = QStringLiteral("Im Gange");    break;
        case AnimationsZustand::Pausiert: text = QStringLiteral("Pausiert");    break;
        case AnimationsZustand::Beendet:  text = QStringLiteral("Beendet");     break;
        }
        m_zustandLabel->setText(text);
    } else {
        // Blinken: Label kurzzeitig leer
        m_zustandLabel->setText(QString());
    }
}
