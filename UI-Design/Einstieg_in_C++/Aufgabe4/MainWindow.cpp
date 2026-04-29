#include "MainWindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <sstream>
#include <algorithm>

/**
 * Autor: Henry Mirus
 * Eingabeparameter: parent - optionales Eltern-Widget.
 * Rueckgabeparameter: keiner.
 * Seiteneffekte: Erzeugt GUI-Elemente und verbindet Enter mit dem Slot.
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QLabel* inputLabel = new QLabel("Eingabe (komma-getrennte Woerter, Enter zum Abschliessen):", this);
    layout->addWidget(inputLabel);

    inputField = new QLineEdit(this);
    layout->addWidget(inputField);

    QLabel* outputLabel = new QLabel("Ausgabe:", this);
    layout->addWidget(outputLabel);

    outputField = new QTextEdit(this);
    outputField->setReadOnly(true);
    layout->addWidget(outputField);

    centralWidget->setLayout(layout);

    connect(inputField, SIGNAL(returnPressed()), this, SLOT(onInputReturn()));

    setWindowTitle("Word Processor");
    resize(500, 300);
}

/**
 * Autor: Henry Mirus
 * Eingabeparameter: keine.
 * Rueckgabeparameter: keiner.
 * Seiteneffekte: keine.
 */
MainWindow::~MainWindow()
{
}

/**
 * Autor: Henry Mirus
 * Eingabeparameter: keine.
 * Rueckgabeparameter: keiner.
 * Seiteneffekte: Leert Eingabe, aktualisiert Liste und Ausgabe.
 */
void MainWindow::onInputReturn()
{
    std::string input = inputField->text().toStdString();
    inputField->clear();

    if (input.empty()) {
        outputField->setText("Fehler: Bitte mindestens ein Wort eingeben.");
        resultList.clear();
        return;
    }

    if (input.length() > 500) {
        outputField->setText("Fehler: Eingabe ist zu lang (Maximum: 500 Zeichen).");
        resultList.clear();
        return;
    }

    processInput(input);

    if (resultList.empty()) {
        outputField->setText("Fehler: Keine gueltigen Woerter gefunden.");
        return;
    }

    displayResult();
}

/**
 * Autor: Henry Mirus
 * Eingabeparameter: input - komma-getrennte Zeichenkette.
 * Rueckgabeparameter: keiner.
 * Seiteneffekte: Schreibt das Ergebnis in resultList.
 */
void MainWindow::processInput(const std::string& input)
{
    resultList.clear();

    std::stringstream ss(input);
    std::string word;
    bool addFront = true;

    while (std::getline(ss, word, ',')) {
        // Trim leading/trailing whitespace
        word.erase(0, word.find_first_not_of(" \t\n\r\f\v"));
        word.erase(word.find_last_not_of(" \t\n\r\f\v") + 1);

        if (!word.empty()) {
            if (addFront) {
                resultList.push_front(word);
            } else {
                resultList.push_back(word);
            }
            addFront = !addFront;
        }
    }
}

/**
 * Autor: Henry Mirus
 * Eingabeparameter: keine.
 * Rueckgabeparameter: keiner.
 * Seiteneffekte: Ueberschreibt den Inhalt des Ausgabefeldes.
 */
void MainWindow::displayResult()
{
    std::string result;
    for (size_t i = 0; i < resultList.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += resultList[i];
    }

    outputField->setText(QString::fromStdString(result));
}
