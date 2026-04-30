#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLineEdit>

/**
 * @brief Konstruktor des Hauptfensters.
 *
 * Autor: Henry Mirus
 *
 * @param parent Zeiger auf ein optionales Eltern-Widget.
 * @return Keine Rückgabe (Konstruktor).
 * @sideeffects Baut die Oberfläche auf und verbindet Enter im Eingabefeld
 *              mit der Verarbeitungsfunktion.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::processCommaSeparatedInput);
}

/**
 * @brief Destruktor des Hauptfensters.
 *
 * Autor: Henry Mirus
 *
 * @return Keine Rückgabe (Destruktor).
 * @sideeffects Gibt den Speicher der generierten UI-Struktur frei.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Liest und verarbeitet eine komma-separierte Wortliste aus der Eingabe.
 *
 * Autor: Henry Mirus
 *
 * Eingabe:
 * - Keine direkten Parameter; die Funktion liest den Inhalt von ui->lineEdit.
 *
 * Rückgabe:
 * - Keine (void).
 *
 * Seiteneffekte:
 * - Leert den bisherigen Inhalt von ui->listWidget.
 * - Schreibt die neu berechnete Wortreihenfolge in ui->listWidget.
 */
void MainWindow::processCommaSeparatedInput()
{
    const QStringList inputWords = ui->lineEdit->text().split(',', Qt::SkipEmptyParts);
    QStringList reorderedWords;
    bool placeNextWordAtFront = true;

    for (const QString &untrimmedWord : inputWords) {
        const QString trimmedWord = untrimmedWord.trimmed();
        if (trimmedWord.isEmpty()) {
            continue;
        }

        if (placeNextWordAtFront) {
            reorderedWords.prepend(trimmedWord);
        } else {
            reorderedWords.append(trimmedWord);
        }
        placeNextWordAtFront = !placeNextWordAtFront;
    }

    ui->listWidget->clear();
    ui->listWidget->addItems(reorderedWords);
}
