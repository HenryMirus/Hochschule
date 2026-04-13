# =============================================================================
# Maschinelles Lernen I
# Teil 1: Python-Grundlagen
# =============================================================================
#
# Willkommen zum ersten Praktikum!
#
# In diesem Teil lernen wir Python kennen – die Sprache, die wir im gesamten
# Kurs verwenden werden. Viele von euch haben Erfahrung mit C++. Python ist
# ähnlich in der Logik, aber deutlich kürzer und ausdrucksstärker in der
# Syntax. Wir werden das im Laufe dieses Tutorials sehen.
#
# Unser Ziel für heute:
#   - Grundlegende Datentypen in Python verstehen
#   - Kontrollstrukturen (if/else, Schleifen) kennenlernen
#   - Eigene Funktionen und Klassen schreiben
#   - Daten aus Textdateien lesen und in Textdateien schreiben
#
# All das sind Werkzeuge, die wir in Teil 2 und 3 direkt einsetzen werden,
# wenn wir Numpy, Matplotlib und lineare Regression behandeln.
# =============================================================================


# =============================================================================
# 1. GRUNDLEGENDE DATENTYPEN
# =============================================================================
#
# None, bool, float, int, str
# In Python gibt es keine explizite Typdeklaration. Der Interpreter erkennt den
# Typ automatisch.
# "print()" gibt Werte auf der Konsole aus – ähnlich wie std::cout in C++.

# Zahlen
x = 42           # int (Ganzzahl)
y = 3.1416       # float (Gleitkommazahl)

print(x)
print(type(x))
print(y)
print(type(y))

# Arithmetik: wie in C++, aber mit einem Unterschied bei der Division!
print(f'7 / 2  = {7 / 2}')    # Ergibt 3.5 (echte Division, nicht ganzzahlig!)
print(f'7 // 2 = {7 // 2}')   # Ganzzahlige Division (Floor Division)
print(f'7 % 2  = {7 % 2}')    # Modulo
print(f'2 ** 8 = {2 ** 8}')   # Potenz (2^8 = 256)

# Zeichenketten (Strings)
name = "Maschinelles Lernen"
greeting = 'Hallo, Welt!'  # Einfache oder doppelte Anführungszeichen

# f-Strings: sehr praktisch für formatierten Output
print(f' Modultitel: {name}')
print(f'Pi gerundet: {y:.2f}')  # :.2f = 2 Nachkommastellen

# String-Operationen
print(f'  Länge vom String: {len(name)}')
print(f'    Großbuchstaben: {name.upper()}')
print(f'Enthält "Learning": {"Learning" in name}')
# Hausaufgabe: Probieren Sie die folgenden Operationen:
# str + str
# str * int
# str.lower(), str.capitalize(), str.strip()
# str.isalpha(), str.isdigit(), str.isalnum(), str.isspace()
# str.count(), str.find(), str.index(), str.replace()

# Boolesche Werte
print(f'Ist der Modulname vom Typ String: {type(name) is str}')
yes = True
no = False
print(f'True and False = {yes and no}')
print(f'True or False  = {yes or no}')
print(f'not True       = {not yes}')

# Listen
# Listen können Elemente verschiedener Typen enthalten und sind veränderlich.
# Wir nutzen sie auch für Warteschlangen (queues) und Stapelspeicher (stacks)
whole_numbers = [1, 2, 3, 4, 5]
gemischt = [42, 'Hallo', y, True]

print(f'      Wert der Liste: {whole_numbers}')
print(f'     Länge der Liste: {len(whole_numbers)}')
print(f'      Erstes Element: {whole_numbers[0]}')  # Indizierung beginnt bei 0
print(f'     Letztes Element: {whole_numbers[-1]}')  # Negativer Index: von hinten
print(f'         Slice [1:3]: {whole_numbers[1:3]}')  # Teilbereich
whole_numbers.append(6)  # Element anhängen
whole_numbers[0] = 99    # Element verändern
print(f'Neuer Wert der Liste: {whole_numbers}')

# Hausaufgabe: Probieren Sie die folgenden Operationen:
# max(list), min(list), sum(list)
# list.clear()
# list.copy()
# list.count()
# list.extend()
# list.index()
# list.insert()
# list.pop()
# list.remove()
# list.reverse()
# list.sort()

# Tupel (unveränderliche Listen)
point = (3.0, 7.5)  # Koordinate (x, y)
print(f'Tupel: {point}, x={point[0]}, y={point[1]}')
# point[0] = 5.0  # Fehler: Tupel sind unveränderlich.

# Sets (Mengen)
colors = set()
colors.add('red')
colors.add('green')
colors.add('green')
colors.add('blue')
colors.add('blue')
print(f'Anzahl gespeicherten Farben: {len(colors)}')
print(f'   Die gespeicherten Farben: {colors}')

# Dictionaries (Schlüssel-Wert-Paare, ähnlich wie std::map)
# Für Schlüssel können wir Datentypen verwenden, für die einen Hash-Wert
# berechnet werden kann.
hyperparameter = {
    'learning rate': 0.01,
    'epochs': 100,
    'batch size': 32
}
print(f' Learning rate: {hyperparameter["learning rate"]}')
hyperparameter['momentum'] = 0.9   # Neues Element hinzufügen
print(f'Alle Schlüssel: {list(hyperparameter.keys())}')
print(f'    Alle Werte: {list(hyperparameter.values())}')
print(f'Alle S-W-Paare: {list(hyperparameter.items())}')

# =============================================================================
# 2. KONTROLLSTRUKTUREN
# =============================================================================

# if / elif / else
# Wichtig: In Python gibt es keine geschweiften Klammern!
# Blöcke werden durch Einrückung (4 Leerzeichen) definiert.

loss = 0.042
if loss < 0.01:
    print('Sehr guter Verlustwert!')
elif loss < 0.1:
    print(f'Akzeptabler Verlust: {loss:.4f}')
else:
    print(f'Verlust noch zu hoch: {loss:.4f}, weiter trainieren.')

# for-Schleifen
# Diese Schleife ist eine "for-each"-Schleife über ein iterierbares Objekt.

print('\nQuadrate von 0 bis 4:')
for i in range(5):        # range(5) erzeugt: 0, 1, 2, 3, 4
    print(f'  {i}^2 = {i**2}')

# Über eine Liste iterieren:
models = ['Lineare Regression', 'KNN', 'SVM', 'Neuronales Netz']
print('Verfügbare Modelle:')
for model in models:
    print(f' - {model}')
# enumerate() liefert Index und Wert gleichzeitig
print('Verfügbare Modelle:')
for index_model, model in enumerate(models):
    print(f' {index_model + 1}. {model}')

# while-Schleifen
# Nützlich z.B. beim Training, wenn wir bis zur Konvergenz iterieren.

print('\nGradientenabstieg (simuliert):')
loss = 10.0
iteration = 0
while loss > 1.0:
    loss *= 0.6   # Verlust wird pro Schritt um 40% reduziert
    iteration += 1
    print(f'  Iteration {iteration}: Verlust = {loss:.4f}')

# List Comprehensions
squares = [i**2 for i in range(6)]
print(f'Quadrate (List Comprehension): {squares}')

# Mit Bedingung (nur gerade Zahlen):
squares_of_even = [i**2 for i in range(10) if i % 2 == 0]
print(f'Quadrate gerader Zahlen: {squares_of_even}')


# =============================================================================
# ÜBUNG 1
# =============================================================================
#
# Schreiben Sie eine List Comprehension, die alle Zahlen von 1 bis 20 enthält,
# die durch 3 teilbar sind.
# Erwartetes Ergebnis: [3, 6, 9, 12, 15, 18]
# Hinweis: "x ist durch 3 teilbar" bedeutet x % 3 == 0

pass


# =============================================================================
# 3. FUNKTIONEN
# =============================================================================

# Funktionen werden mit dem Schlüsselwort "def" definiert.
# Keine Typangaben nötig (obwohl man sie optional hinzufügen kann).

def greeting(name, sprache='Deutsch'):
    """
    Gibt eine Begrüssung aus.

    :param name: Name der Person.
    :param sprache: Sprache der Begrüssung (Standard: Deutsch).
    :return: Begrüssungszeichenkette.
    """
    if sprache == 'Deutsch':
        return f'Hallo, {name}!'
    elif sprache == 'Norddeutsch':
        return f'Moin, {name}!'
    else:
        return f'Servus, {name}!'

print(greeting('Alice'))
print(greeting('Bob', sprache='Bayerisch'))

# Funktionen für ML: Verlustfunktionen
# Schreiben wir Squared Error als Python-Funktion.

def squared_error(y_true: float, y_predicted: float):
    """
    Berechnet den quadratischen Fehler zwischen wahrem und vorhergesagtem Wert.
    
    :param y_true: wahrer Wert.
    :param y_predicted: vorhergesagter Wert.
    :return: Quadratischer Fehler (y_true - y_predicted)^2
    """
    return (y_true - y_predicted) ** 2


def mean_squared_error(y_true, y_predicted):
    """
    Berechnet den mittleren quadratischen Fehler (MSE) über eine Liste von Werten.
    
    MSE = (1/n) * sum((y_i - ŷ_i)^2)
    
    :param y_true: Liste der wahren Werte.
    :param y_predicted: Liste der vorhergesagten Werte.
    :return: Mittlerer quadratischer Fehler.
    """
    n = len(y_true)
    total_error = 0.0
    for i in range(n):
        total_error += squared_error(y_true[i], y_predicted[i])
    return total_error / n


# Test der Funktionen
true_values = [2.0, 4.0, 6.0, 8.0]
predictions = [2.5, 3.8, 6.2, 7.5]

mse = mean_squared_error(true_values, predictions)
print(f'Wahre Werte: {true_values}')
print(f'Vorhersagen: {predictions}')
print(f'        MSE: {mse:.4f}')

# Mehrere Rückgabewerte
# Python-Funktionen können mehrere Werte zurückgeben (als Tupel).
# Das werden wir beim Gradientenabstieg nutzen!

def min_max(values: list) -> tuple:
    """Gibt Minimum und Maximum einer Liste zurück."""
    return min(values), max(values)

minimum, maximum = min_max([3, 1, 7, 2, 9, 4])
print(f'Minimum: {minimum}, Maximum: {maximum}')


# =============================================================================
# ÜBUNG 2
# =============================================================================
#
# Schreiben Sie eine Funktion "absolute_error(y_true, y_predicted)", die den
# absoluten Fehler |y_true - y_pred| berechnet und zurückgibt.
# Schreiben Sie eine Funktion "mean_absolute_error(y_true, y_predicted)", die
# den mittleren absoluten Fehler (MAE) berechnet:
#
#   MAE = (1/n) * sum(|y_i - ŷ_i|)
#
# Testen Sie sie mit den obigen Listen. Erwartetes Ergebnis: MAE = 0.35

pass

# =============================================================================
# 4. KLASSEN
# =============================================================================

# In Python werden Klassen mit "class" definiert.
# Die Methode __init__ ist der Konstruktor.
# "self" entspricht "this" in C++ – muss aber explizit als erster Parameter
# jeder Methode angegeben werden.

class DataPoint:
    """
    Repräsentiert einen einzelnen Datenpunkt mit einem Feature x und einem Label y.
    So ein Datenpunkt könnte z.B. sein:
      x = Wohnfläche in m²
      y = Mietpreis in Euro
    """

    def __init__(self, x, y):
        """Konstruktor: Erzeugt einen Datenpunkt mit Feature x und Label y."""
        self.x = x
        self.y = y

    def __repr__(self):
        """Wird aufgerufen, wenn man den Datenpunkt druckt (wie operator<< in C++)."""
        return f'DataPoint(x={self.x:.2f}, y={self.y:.2f})'


class Dataset:
    """
    Repräsentiert einen Datensatz aus mehreren Datenpunkten.
    Bietet grundlegende Statistik- und Hilfsmethoden.
    """

    def __init__(self, datapoints=None):
        """
        Konstruktor.
        
        :param datenpunkte: Optionale Anfangsliste von DataPoint-Objekten.
        """
        if datapoints is None:
            self._datapoints = []
        else:
            self._datapoints = datapoints

    def __len__(self):
        """Gibt die Anzahl der Datenpunkte zurück."""
        return len(self._datapoints)

    def __repr__(self):
        return f'Datensatz mit {len(self)} Datenpunkten'

    def append(self, datapoint: DataPoint):
        """Fügt einen Datenpunkt zum Datensatz hinzu."""
        self._datapoints.append(datapoint)

    def x_values(self) -> list:
        """Gibt alle x-Werte als Liste zurück."""
        return [point.x for point in self._datapoints]

    def y_values(self) -> list:
        """Gibt alle y-Werte als Liste zurück."""
        return [point.y for point in self._datapoints]

    def mean_x(self):
        """Berechnet den Mittelwert der x-Werte."""
        return sum(self.x_values()) / len(self)

    def mean_y(self):
        """Berechnet den Mittelwert der y-Werte."""
        return sum(self.y_values()) / len(self)


# Erstellen wir einen kleinen Beispieldatensatz.
# Wir messen den Zusammenhang zwischen Lernstunden und Klausurergebnis.
ds = Dataset()
ds.append(DataPoint(2.0, 42.0))
ds.append(DataPoint(3.5, 55.0))
ds.append(DataPoint(5.0, 61.0))
ds.append(DataPoint(6.5, 70.0))
ds.append(DataPoint(8.0, 78.0))
ds.append(DataPoint(9.5, 85.0))

print(ds)
print(f'Mittelwert x: {ds.mean_x():.2f}')
print(f'Mittelwert y: {ds.mean_y():.2f}')
print(f'     x-Werte: {ds.x_values()}')


# =============================================================================
# ÜBUNG 3
# =============================================================================
#
# Fügen Sie der Klasse "Dataset" eine Methode "standard_deviation_y()" hinzu,
# die die Standardabweichung der y-Werte berechnet:
#   std = sqrt( (1/n) * sum((y_i - mean_y)^2) )
#
# Hinweis: sqrt gibt es als math.sqrt() – importieren Sie das Modul mit:
#   import math
#
# Testen Sie die neue Methode mit dem Datensatz ds.

import math

pass

# =============================================================================
# 5. DATEIEN LESEN UND SCHREIBEN
# =============================================================================

# In der Praxis lesen wir Datensätze aus Textdateien, z.B. CSV.
# Python bietet dafür sehr einfache Syntax.

# Schreiben in eine Textdatei
# "with"-Block: Die Datei wird automatisch geschlossen, auch bei Fehlern.
# Modus "w" = write (überschreibt), "a" = append, "r" = read

file_csv = 'lernstunden.csv'  # Bitte Pfad anpassen

with open(file_csv, 'w') as file_handle:
    file_handle.write('"Stunden","Ergebnis"\n')   # Kopfzeile (Header)
    for point in ds._datapoints:
        file_handle.write(f'{point.x},{point.y}\n')

print(f'Datei "{file_csv}" wurde geschrieben.')

# Lesen aus einer Textdatei

data_points = []
with open(file_csv, 'r') as file_handle:
    for index_line, line in enumerate(file_handle):
        if index_line == 0:
            continue  # Kopfzeile ignorieren
        values = line.strip().split(',')  # An Kommas splitten
        x = float(values[0])   # str -> float
        y = float(values[1])
        data_points.append(DataPoint(x, y))

print(f'Eingelesene Datenpunkte: {len(data_points)}')
for point in data_points:
    print(f'  {point}')


# =============================================================================
# ÜBUNG 4
# =============================================================================
#
# Schreiben Sie die Funktion "load_csv(dateiname)", die eine CSV-Datei einliest
# (mit einer Kopfzeile) und einen Datensatz als Dataset-Objekt zurückgibt.
#
# Testen Sie die Funktion, indem Sie damit "lernstunden.csv" laden und den
# Mittelwert der y-Werte ausgeben. Das Ergebnis sollte ~ 65.17 sein.

pass
