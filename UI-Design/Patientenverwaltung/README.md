# Patientenverwaltung

Autor: Henry Mirus

## KI-Disclaimer
Github-Copilot Pro wurde verwendet, um Funktionsbeschreibungen zu verfassen, Übersichtlichkeit in der Struktur des Skriptes zu gewährleisten und Verbesserungen (hinsichtlich Exception-Handling und Edge-Cases) einzubauen.

## Inhalt
Dieses Projekt implementiert eine kleine Patientenverwaltung mit Qt.

## Funktionen
- Patienten anlegen, ansehen, bearbeiten und löschen
- Suche nach Name, Vorname und Geburtsdatum
- Sortierte Anzeige der Patientenliste
- Laden und Speichern der Daten als JSON

## Funktionsweise
Beim Start der Anwendung wird im Verzeichnis des Programms automatisch die Datei `patients.json` geladen. Enthält die Datei bereits Patientendaten, werden diese in die interne Liste übernommen und direkt in der Tabellenansicht angezeigt. Die Anzeige ist dabei bewusst auf die wichtigsten Informationen reduziert: Nachname, Vorname und Geburtsdatum. Die vollständigen Daten eines Patienten werden erst in den Dialogen zum Anzeigen oder Bearbeiten sichtbar.

Die Hauptoberfläche besteht aus einer Tabelle, einem Suchfeld und den Schaltflächen zum Anlegen, Anzeigen, Bearbeiten, Löschen und Zurücksetzen der Auswahl. Die Patientenliste wird nicht direkt in der Oberfläche gespeichert, sondern über eine separate Verwaltungslogik verwaltet. Dadurch bleibt die Benutzeroberfläche übersichtlich und die fachlichen Operationen wie Anlegen, Ändern und Löschen sind klar von der Darstellung getrennt. Zusätzlich sorgt die Verwaltungsklasse dafür, dass die Liste sortiert bleibt, sodass Patienten immer in einer einheitlichen Reihenfolge erscheinen.

Über das Suchfeld kann die Liste gefiltert werden. Die Suche arbeitet exakt und berücksichtigt Name, Vorname und Geburtsdatum. Mehrere Suchbegriffe können mit Leerzeichen oder Komma getrennt werden. Dabei dürfen höchstens drei Suchparameter verwendet werden, weil die Suche nur auf diese drei Datenfelder ausgelegt ist. Wird ein gültiger Suchtext eingegeben, baut das Programm die sichtbare Tabelle neu auf und zeigt nur die passenden Patienten an.

Ein neuer Patient wird über einen Eingabedialog erfasst. Dort werden die Stammdaten vollständig eingetragen und bei Bedarf geprüft, bevor der Dialog geschlossen wird. Das Bearbeiten funktioniert nach demselben Prinzip, nur dass bereits vorhandene Daten in die Eingabefelder übernommen werden. Das reine Anzeigen eines Datensatzes öffnet einen Dialog ohne Bearbeitungsmöglichkeiten. Beim Löschen fragt das Programm zur Sicherheit noch einmal nach, damit keine Daten versehentlich entfernt werden.

Alle Änderungen wirken sich sofort auf die interne Liste und damit auf die Tabelle aus. Sobald das Hauptfenster geschlossen wird, speichert das Programm die aktuelle Patientenliste wieder als JSON-Datei. Dadurch bleiben neu angelegte oder geänderte Patienten auch nach dem Beenden erhalten. Das Programm ist damit als einfache, lokale Desktop-Anwendung aufgebaut, die ohne zusätzliche Server oder Datenbank auskommt.

## Daten
Ein Patient besteht aus folgenden Angaben:
- Titel
- Nachname
- Vorname
- Straße
- Ort
- Telefonnummer
- Geburtsdatum
- Geschlecht

## Build
Das Projekt ist ein Qt/CMake-Projekt. Der Build erfolgt in der vorhandenen Qt-Umgebung über CMake bzw. Qt Creator.

## Dateien
- `mainwindow.*` für die Hauptoberfläche und die Aktionen
- `patientapi.*` für die CRUD-Logik
- `patientjsonstorage.*` für das JSON-Laden und -Speichern
- `patienttablemodel.*` für die Tabellenanzeige
- `patienteditdialog.*` und `patientviewdialog.*` für Bearbeiten und Anzeigen

## Patientendantensatz
[
    {
        "birthday": "2000-01-01",
        "city": "",
        "firstName": "A",
        "gender": "",
        "lastName": "A",
        "phone": "",
        "street": "",
        "title": ""
    },
    {
        "birthday": "2000-01-01",
        "city": "",
        "firstName": "a",
        "gender": "",
        "lastName": "a",
        "phone": "",
        "street": "",
        "title": ""
    },
    {
        "birthday": "1111-11-11",
        "city": "",
        "firstName": "b",
        "gender": "",
        "lastName": "A",
        "phone": "",
        "street": "",
        "title": ""
    },
    {
        "birthday": "2001-12-12",
        "city": "",
        "firstName": "b",
        "gender": "",
        "lastName": "a",
        "phone": "",
        "street": "",
        "title": ""
    },
    {
        "birthday": "2000-01-01",
        "city": "",
        "firstName": "aa",
        "gender": "",
        "lastName": "aa",
        "phone": "",
        "street": "",
        "title": ""
    },
    {
        "birthday": "2000-01-01",
        "city": "",
        "firstName": "a",
        "gender": "",
        "lastName": "b",
        "phone": "",
        "street": "",
        "title": ""
    },
    {
        "birthday": "2001-01-01",
        "city": "",
        "firstName": "B",
        "gender": "",
        "lastName": "B",
        "phone": "",
        "street": "",
        "title": ""
    },
    {
        "birthday": "1999-01-01",
        "city": "",
        "firstName": "C",
        "gender": "",
        "lastName": "C",
        "phone": "",
        "street": "",
        "title": ""
    },
    {
        "birthday": "2005-09-18",
        "city": "53567 Buchholz",
        "firstName": "Mirus",
        "gender": "Männlich",
        "lastName": "Henry",
        "phone": "015750126303",
        "street": "Pantaleonstraße 20",
        "title": ""
    },
    {
        "birthday": "2000-01-01",
        "city": "12345 Musterstadt",
        "firstName": "Mustermann",
        "gender": "Männlich",
        "lastName": "Max",
        "phone": "0123456789",
        "street": "Musterstraße 1",
        "title": "Dr."
    }
]
