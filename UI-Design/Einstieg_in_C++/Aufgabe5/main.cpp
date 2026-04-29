#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

/**
 * Autor: Henry Mirus
 * Zweck: Diese Klasse stellt das Hauptfenster fuer Aufgabe 5 bereit.
 * Sie liest Vor- und Nachnamen ein, speichert sie nach Nachnamen sortiert
 * in einem Container und zeigt sie auf Wunsch in einer MessageBox an.
 */
class NameWindow : public QWidget
{
    Q_OBJECT

public:
    /**
        * Autor: Henry Mirus
     * Eingabeparameter: parent - optionales Eltern-Widget.
     * Rueckgabeparameter: keiner.
     * Seiteneffekte: Erzeugt und konfiguriert alle Widgets des Fensters.
     */
    explicit NameWindow(QWidget* parent = nullptr)
        : QWidget(parent)
        , outputLabel(new QLabel("Geben Sie Vor- und Nachnamen ein:", this))
        , inputField(new QLineEdit(this))
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(outputLabel);
        layout->addWidget(inputField);

        setLayout(layout);
        setWindowTitle("Aufgabe 5 - Namen sortieren");
        resize(500, 140);

        connect(inputField, &QLineEdit::returnPressed, this, &NameWindow::onReturnPressed);
    }

private slots:
    /**
        * Autor: Henry Mirus
     * Eingabeparameter: keine.
     * Rueckgabeparameter: keiner.
     * Seiteneffekte: Liest den Inhalt des Eingabefelds, validiert ihn und
     * aktualisiert den Namenscontainer oder zeigt die sortierte Ausgabe an.
     */
    void onReturnPressed()
    {
        const QString rawInput = inputField->text().trimmed();

        if (rawInput.isEmpty()) {
            showAllNames();
            return;
        }

        const QStringList parts = rawInput.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() != 2) {
            QMessageBox::warning(
                this,
                "Ungueltige Eingabe",
                "Bitte genau einen Vornamen und einen Nachnamen eingeben.");
            return;
        }

        const QString firstName = parts[0];
        const QString lastName = parts[1];

        if (firstName.size() > 50 || lastName.size() > 50) {
            QMessageBox::warning(
                this,
                "Ungueltige Eingabe",
                "Vorname und Nachname duerfen jeweils maximal 50 Zeichen lang sein."
            );
            return;
        }

        if (!isValidNamePart(firstName) || !isValidNamePart(lastName)) {
            QMessageBox::warning(
                this,
                "Ungueltige Eingabe",
                "Erlaubt sind Buchstaben und Bindestriche (z. B. Anna-Maria)."
            );
            return;
        }

        if (sortedNamesByLastName.contains(lastName)) {
            QMessageBox::warning(
                this,
                "Doppelter Nachname",
                "Dieser Nachname ist bereits vorhanden und wird nicht erneut gespeichert."
            );
            return;
        }

        sortedNamesByLastName[lastName] = firstName;
        inputField->clear();
    }

private:
    /**
        * Autor: Henry Mirus
     * Eingabeparameter: namePart - zu pruefender Namensbestandteil.
     * Rueckgabeparameter: true bei gueltigem Namen, sonst false.
     * Seiteneffekte: keine.
     */
    bool isValidNamePart(const QString& namePart) const
    {
        static const QRegularExpression pattern(
            "^[\\p{L}]+(-[\\p{L}]+)*$",
            QRegularExpression::UseUnicodePropertiesOption
        );
        return pattern.match(namePart).hasMatch();
    }

    /**
        * Autor: Henry Mirus
     * Eingabeparameter: keine.
     * Rueckgabeparameter: keiner.
     * Seiteneffekte: Zeigt alle gespeicherten Namen als "Nachname, Vorname"
     * zeilenweise in einer QMessageBox an.
     */
    void showAllNames()
    {
        if (sortedNamesByLastName.isEmpty()) {
            QMessageBox::information(this, "Sortierte Namen", "Keine Namen vorhanden.");
            return;
        }

        QStringList lines;
        for (QMap<QString, QString>::const_iterator it = sortedNamesByLastName.cbegin();
             it != sortedNamesByLastName.cend();
             ++it) {
            lines << QString("%1, %2").arg(it.key(), it.value());
        }

        QMessageBox::information(this, "Sortierte Namen", lines.join("\n"));
    }

    QLabel* outputLabel;
    QLineEdit* inputField;
    QMap<QString, QString> sortedNamesByLastName;
};

/**
 * Autor: Henry Mirus
 * Eingabeparameter: argc, argv - Programmargumente.
 * Rueckgabeparameter: Exit-Code der Qt-Applikation.
 * Seiteneffekte: Startet Event-Loop und zeigt das Hauptfenster an.
 */
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    NameWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
