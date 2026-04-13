# =============================================================================
# Maschinelles Lernen I
# Teil 2: Numpy und Datenvisualisierung
# =============================================================================
#
# In Teil 1 haben wir Python mit Listen und einfachen Schleifen verwendet.
# Das funktioniert, ist aber bei grossen Datensätzen sehr langsam:
# Stellen Sie sich vor, ihr habt 1 Million Datenpunkte und wollen jedes Element
# quadrieren – eine Python-for-Schleife würde Sekunden dauern.
#
# Numpy löst dieses Problem. Es ist eine Bibliothek für numerische Berechnungen,
# die intern in C und Fortran implementiert ist und auf ganzen Arrays
# gleichzeitig arbeitet (vektorisierte Operationen). Das ist um Grössenordnungen
# schneller.
#
# In diesem Teil lernen wir:
#   - Numpy-Arrays erstellen und manipulieren
#   - Vektor- und Matrixoperationen mit Numpy
#   - Grundlegende Statistik mit Numpy
#   - Datensätze aus CSV-Dateien laden (Numpy-Variante)
#   - Daten mit Matplotlib visualisieren
# =============================================================================

import matplotlib.pyplot as plt
import numpy as np

# =============================================================================
# 1. NUMPY-ARRAYS: DIE GRUNDLAGE
# =============================================================================

# Ein Numpy-Array erstellen aus einer Python-Liste:
v = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
print(f'    Vektor v: {v}')
print(f'         Typ: {type(v)}')
print(f'  Elementtyp: {v.dtype}')  # float64 = 64-Bit Gleitkommazahl
print(f'Shape (Form): {v.shape}')  # (5,) = 1D-Array mit 5 Elementen
print(f'       Länge: {len(v)}')  # oder v.size

# Arrays erzeugen ohne manuelle Liste
nullen    = np.zeros(5)           # [0. 0. 0. 0. 0.]
einsen    = np.ones(5)            # [1. 1. 1. 1. 1.]
bereich   = np.arange(0, 10, 2)   # [0 2 4 6 8] – wie range(), aber als Array
linear    = np.linspace(0, 1, 5)  # 5 gleichmässige Punkte von 0 bis 1 (inklusiv!)
zufaellig = np.random.randn(5)    # 5 Zufallszahlen aus N(0,1)

print(f'         np.zeros(5): {nullen}')
print(f'          np.ones(5): {einsen}')
print(f' np.arange(0, 10, 2): {bereich}')
print(f'np.linspace(0, 1, 5): {linear}')
print(f'  np.random.randn(5): {zufaellig}')

# Matrizen (2D-Arrays)
# In ML arbeiten wir oft mit Matrizen: z.B. Design-Matrix X (n x d)
M = np.array([
    [1.0, 2.0, 3.0],
    [4.0, 5.0, 6.0]
])
print(f'Matrix M:\n{M}')
print(f'Shape: {M.shape}')     # (2, 3) = 2 Zeilen, 3 Spalten
print(f'M[0, :] = {M[0, :]}')  # Erste Zeile
print(f'M[:, 1] = {M[:, 1]}')  # Zweite Spalte


# =============================================================================
# 2. VEKTORISIERTE OPERATIONEN – DER GROSSE VORTEIL VON NUMPY
# =============================================================================

# In Python müssten wir eine Schleife schreiben:
# ergebnis = [x**2 for x in v]
#
# Mit Numpy geht das direkt auf dem ganzen Array – viel schneller!

a = np.array([1.0, 2.0, 3.0, 4.0])
b = np.array([10.0, 20.0, 30.0, 40.0])

print(f'      a = {a}')
print(f'      b = {b}')
print(f'  a + b = {a + b}')      # Elementweise Addition
print(f'  a * b = {a * b}')      # Elementweise Multiplikation (KEIN Skalarprodukt!)
print(f' a ** 2 = {a ** 2}')     # Elementweise Quadrierung
print(f'  a * 3 = {a * 3}')      # Skalarmultiplikation
print(f'a + 100 = {a + 100}')    # Scalar Broadcasting

# Skalarprodukt (Dot Product)
# Das Skalarprodukt a·b = sum(a_i * b_i) ist fundamental in ML.
# Beispiel: y = w^T * x ist die Vorhersage eines linearen Modells.

dot_produkt = np.dot(a, b)
dot_alternativ = a @ b           # @ ist der Matmul-Operator
print(f'a · b = {dot_produkt}')
print(f'a @ b = {dot_alternativ}')

# Mathematische Funktionen
x_values = np.linspace(-np.pi, np.pi, 5)
print(f'Sinus von {x_values}: {np.sin(x_values)}')
print(f'Exponential:         {np.exp(np.array([0, 1, 2]))}')
print(f'Logarithmus:         {np.log(np.array([1, np.e, np.e**2]))}')


# =============================================================================
# 3. STATISTIK MIT NUMPY
# =============================================================================

# Zufälligen Datensatz erzeugen (reproduzierbar mit seed!)
np.random.seed(19751979)  # Seed setzen für Reproduzierbarkeit
random_set = np.random.randn(100) * 15 + 65  # Normalverteilt: mu=65, sigma=15

print(f'Anzahl Datenpunkte: {len(random_set)}')
print(f'        Mittelwert: {np.mean(random_set):.2f}')
print(f'Standardabweichung: {np.std(random_set):.2f}')
print(f'           Minimum: {np.min(random_set):.2f}')
print(f'           Maximum: {np.max(random_set):.2f}')
print(f'            Median: {np.median(random_set):.2f}')

# Aggregation entlang einer Achse bei Matrizen:
M2 = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]], dtype=np.int32)
print(f'Matrix M2:\n{M2}')
print(f'Spaltenmittelwerte (axis=0): {np.mean(M2, axis=0)}')  # Über Zeilen
print(f' Zeilenmittelwerte (axis=1): {np.mean(M2, axis=1)}')  # Über Spalten


# =============================================================================
# ÜBUNG 5
# =============================================================================
#
# Erzeugen Sie mit np.random.randn() und np.random.seed(7) einen Vektor mit
# 50 Zufallszahlen aus N(0, 1).
# Berechnen Sie dann:
#   a) Den Mittelwert und die Standardabweichung
#   b) Die Anzahl der Werte, die grösser als 0 sind
#      (Hinweis: numpy-Vergleiche erzeugen Boolean-Arrays; True = 1, False = 0,
#       also: np.sum(array > 0) zählt die True-Werte)
#   c) Den maximalen absoluten Wert im Array
#      (Hinweis: np.abs() berechnet den Betrag elementweise)

pass


# =============================================================================
# 4. CSV-DATEIEN MIT NUMPY LADEN
# =============================================================================

# In Teil 1 haben wir CSV-Dateien "von Hand" geparst.
# Numpy kann das viel einfacher mit np.loadtxt() erledigen.

# Wir erstellen zuerst unseren echten Datensatz:
# Werbeausgaben (TV, Radio, Newspaper in Tausend USD) und
# Verkaufszahlen (in Tausend Einheiten)
# Quelle: "Introduction to Statistical Learning"
#
# Wir verwenden eine Teilmenge des Datensatzes (20 Zeilen) für dieses Tutorial.
# Die Frage: Wie gut lässt sich der Absatz durch das TV-Werbebudget vorhersagen?

# Datei erzeugen (in der echten Sitzung würde diese Datei bereits vorliegen):
advertising_data = np.array([
    [230.1, 37.8, 69.2, 22.1],
    [ 44.5, 39.3, 45.1, 10.4],
    [ 17.2, 45.9, 69.3,  9.3],
    [151.5, 41.3, 58.5, 18.5],
    [180.8, 10.8, 58.4, 12.9],
    [  8.7, 48.9, 75.0,  7.2],
    [ 57.5, 32.8, 23.5, 11.8],
    [120.2, 19.6, 11.6, 13.2],
    [  8.6,  2.1,  1.0,  4.8],
    [199.8,  2.6, 21.2, 10.6],
    [ 66.1,  5.8, 24.2,  8.6],
    [214.7, 24.0,  4.0, 17.4],
    [ 23.8, 35.1, 65.9,  9.2],
    [ 97.5,  7.6,  7.2,  9.7],
    [204.1, 32.9, 46.0, 19.0],
    [195.4, 47.7, 52.9, 22.4],
    [ 67.8, 36.6,114.0, 12.5],
    [281.4, 39.6, 55.8, 24.4],
    [ 69.2, 20.5, 18.3, 11.3],
    [147.3, 23.9, 19.1, 14.6],
])

np.savetxt('advertising.csv', advertising_data,
           delimiter=',',
           header='TV,Radio,Newspaper,Sales',
           comments='')    # verhindert das "#" vor dem Header

print('Datei "advertising.csv" wurde erzeugt.')

# Jetzt laden wir sie mit numpy:
advertising_loaded = np.loadtxt('advertising.csv',
                      delimiter=',',
                      skiprows=1)    # Header überspringen

print(f'Shape der geladenen Daten: {advertising_loaded.shape}')
print(f'Erste 3 Zeilen:\n{advertising_loaded[:3, :]}')

# Spalten extrahieren:
tv      = advertising_loaded[:, 0]   # Erste Spalte: TV-Budget
sales   = advertising_loaded[:, 3]   # Vierte Spalte: Verkaufszahlen

print(f'TV-Budget (erste 5 Werte): {tv[:5]}')
print(f'Verkäufe  (erste 5 Werte): {sales[:5]}')
print(f' Korrelation TV <-> Sales: {np.corrcoef(tv, sales)[0,1]:.4f}')


# =============================================================================
# 5. DATENVISUALISIERUNG MIT MATPLOTLIB
# =============================================================================

# Matplotlib ist die Standard-Bibliothek für Plots in Python.
# pyplot ist das Submodul für vordefinierte Plots (ähnlich wie MATLAB-Syntax).
# Figur erstellen -> Daten plotten -> beschriften -> anzeigen/speichern

# Plot 1: Einfacher Scatter Plot
# Visualisierung: TV-Werbebudget vs. Verkaufszahlen

_, ax = plt.subplots(figsize=(8, 5))   # Figur und Achsenobjekt erstellen

ax.scatter(tv, sales,                     # x-Werte, y-Werte
           color='steelblue',
           alpha=0.7,                     # Transparenz (0=unsichtbar, 1=voll)
           edgecolors='white',
           s=80,                          # Punktgrösse
           label='Datenpunkte')

ax.set_xlabel('TV-Werbebudget (Tausend USD)', fontsize=12)
ax.set_ylabel('Verkaufszahlen (Tausend Einheiten)', fontsize=12)
ax.set_title('TV-Werbebudget vs. Verkaufszahlen', fontsize=14)
ax.legend()
ax.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('scatter_tv_sales.png', dpi=100)
print('Plot 1 gespeichert')
plt.show()

# Plot 2: Mehrere Subplots
# Wir wollen alle drei Kanäle (TV, Radio, Newspaper) auf einmal sehen.

radio = advertising_loaded[:, 1]
newspaper = advertising_loaded[:, 2]

_, axes = plt.subplots(1, 3, figsize=(15, 4))   # 1 Zeile, 3 Spalten

kanal_daten = [(tv, 'TV', 'steelblue'),
               (radio, 'Radio', 'darkorange'),
               (newspaper, 'Zeitung', 'forestgreen')]

for ax, (x_data, name, farbe) in zip(axes, kanal_daten):
    ax.scatter(x_data, sales, color=farbe, alpha=0.7, edgecolors='white', s=70)
    ax.set_xlabel(f'{name}-Budget (Tsd. USD)', fontsize=11)
    ax.set_ylabel('Verkäufe (Tsd. Einheiten)', fontsize=11)
    ax.set_title(f'{name} vs. Verkäufe', fontsize=12)
    ax.grid(True, alpha=0.3)
    
    # Korrelation in den Plot schreiben:
    r = np.corrcoef(x_data, sales)[0, 1]
    style = dict(boxstyle='round,pad=0.3', facecolor='lightyellow', alpha=0.8)
    ax.text(0.05, 0.92, f'r = {r:.2f}',
        transform=ax.transAxes,
        fontsize=11, color='black',
        bbox=style)

plt.suptitle('Werbekanäle vs. Verkaufszahlen', fontsize=14, fontweight='bold')
plt.tight_layout()
plt.savefig('scatter_alle_kanaele.png', dpi=150)
print('Plot 2 gespeichert')
plt.show()


# =============================================================================
# ÜBUNG 6
# =============================================================================
#
# Erstellen Sie einen Histogramm-Plot der TV-Werbebudgets mit ax.hist().
# Verwenden Sie 8 Bins. Beschriften Sie die Achsen sinnvoll und geben Sie dem
# Plot einen Titel. Speichern Sie den Plot als "histogramm_tv.png".
#
# Hinweis: ax.hist(daten, bins=8, color='steelblue', edgecolor='white')

pass


# =============================================================================
# 6. NUMPY UND LINEARE ALGEBRA
# =============================================================================

# Das werden wir in Teil 3 für die analytische Lösung der linearen Regression
# brauchen. Kurze Wiederholung der wichtigsten Operationen.

A = np.array([[2.0, 1.0],
              [1.0, 3.0]])
b_vec = np.array([5.0, 10.0])

print(f'Matrix A:\n{A}')
print(f'Vektor b: {b_vec}')

# Transponieren:
print(f'A^T:\n{A.T}')

# Matrixmultiplikation:
print(f'A @ A^T:\n{A @ A.T}')

# Inverse:
A_inv = np.linalg.inv(A)
print(f'A^(-1):\n{A_inv}')
print(f'A @ A^(-1) ≈ I:\n{A @ A_inv}')   # Sollte Einheitsmatrix ergeben

# Lineares Gleichungssystem lösen: Ax = b
# Statt x = A^{-1} b ist np.linalg.solve numerisch stabiler!
x_lsg = np.linalg.solve(A, b_vec)
print(f'Lösung von Ax = b: x = {x_lsg}')
print(f'Probe: A @ x = {A @ x_lsg}  (sollte {b_vec} sein)')


# =============================================================================
# ÜBUNG 7
# =============================================================================
#
# Berechnen Sie mit numpy die Norm (Länge) des Vektors v = [3, 4]:
#   ||v|| = sqrt(3^2 + 4^2) = 5
#
# a) Berechnen Sie sie "von Hand" mit numpy-Operationen (ohne np.linalg.norm)
# b) Berechnen Sie sie mit np.linalg.norm()
# c) Normalisieren Sie den Vektor v, sodass er die Länge 1 hat:
#    v_norm = v / ||v||
#    Überprüfen Sie, dass np.linalg.norm(v_norm) ≈ 1.0

pass
