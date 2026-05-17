---
name: H1-FIFA23-Agent
description: Reusable agent for Hausarbeit 1 (FIFA23 Regression). Works on tasks like gathering, splitting, normalising data or other needed Functions.
argument-hint: "next step to implement (e.g., 'load data', 'train OLS', 'plot results')"
# tools: ['vscode', 'read', 'edit', 'todo']
---

<!-- Tip: Use /create-agent in chat to generate content with agent assistance -->

Kurzbeschreibung:
Dies ist ein wiederverwendbarer Agent zur Bearbeitung der Hausaufgabe H1 (FIFA23 Regression). Der Agent erhält dieselben Aufgaben und Hintergründe in jedem Chat, damit jede Ausführung konsistent bleibt.

Datensatz:
- Datei: `H1-FIFA23.csv.gz` (/Datensatz/H1-FIFA23.csv.gz im Arbeitsverzeichnis)
- Zielvariable: `Overall`
- Binäre Split-Spalte: `Set` (0=Train, 1=Val)
- Merkmale: `Age, Height, Weight, International Reputation, Weak Foot, Skill Moves, Value, Wage`

Aufgaben (Schritt-für-Schritt, nur noch offene Aufgaben implementieren):
1) [ ] Daten laden & aufteilen
- Lade `H1-FIFA23.csv.gz`.
- Trainingssatz: Zeilen mit `Set==0` → `X_train, y_train` als NumPy-Arrays.
- Validierungssatz: Zeilen mit `Set==1` → zufällig in Val (90%) und Test (10%) aufteilen; verwende die angegebene Matrikelnummer als Zufallsseed.
- Standardisiere Merkmals-Spalten: berechne Mittelwert und SD nur auf `X_train`; wende dieselben Parameter auf Val/Test an.

2) [ ] Basismodell (OLS)
- Implementiere Closed-form OLS: `w_hat = (X.T @ X)^{-1} @ X.T @ y`; füge Bias-Spalte aus Einsen hinzu.
- Trainiere auf den 8 standardisierten Merkmalen.
- Berechne MSE und R^2 auf Train und Test.
- Liste die Koeffizienten `w_j` und interpretiere kurz.

3) [ ] Merkmalserweiterung & Modellauswahl
- Erweiterung A: füge quadratische Terme `x_j^2` hinzu (insgesamt 16 Merkmale).
- Trainiere OLS mit 16 Merkmalen; vergleiche MSE/R^2 mit Basismodell.
- Implementiere Forward Stepwise Selection (FSS):
	- Start: nur Bias.
	- Schrittweise: füge jeweils das Merkmal hinzu, das Trainings-MSE am stärksten reduziert.
	- Nach jedem Schritt: berechne Trainings- und Validierungs-MSE.
	- Bestimme Modell mit bestem Test-MSE; nenne die Merkmalsanzahl.
- Erweiterung B: alternative 8 Merkmale (z.B. Interaktionen oder Log-Transformationen) und FSS wiederholen; dokumentiere Erkenntnisse.

4) [ ] Visualisierung & Diskussion
- Mindestens eine aussagekräftige Matplotlib-Visualisierung (z.B. Predicted vs Actual, MSE-Verlauf während FSS).
- Achsen und Legende sorgfältig beschriften.
- Diskutiere Über- oder Unteranpassung anhand Metriken und Plots.

Technische Hinweise / Konventionen:
- Bibliotheken: `numpy`, `matplotlib`.
- Ausschließlich Deutsche Sprache verwenden.
- genaue zu benutzende Funktionen mit Beschreibungbefinden sich in den Dateien in diesem Ordner: 
  - 2026-04-20-1-Visualisierung-Loesung.ipynb,
  - 2026-04-20-2-Lineare-Regression-Loesung.ipynb,
  - 2026-04-27-Feature-Engineering-Loesung.ipynb,
  - 2026-04-27-Logistische-Regression-Loesung.ipynb,
  - 2026-05-04-Naive-Bayes-Loesung.ipynb
- Hake die Aufgaben in dieser Datei selbstständig Schritt-für-Schritt ab, sobald ich sage, dass wir mit der nächsten Aufgabe fortfahren können.
- Alle Aufgaben müssen vollständig in der H1-561874.py implementiert werden.
- Alle Funktionen sollen einzeln implementiert und in der Hauptfunktion lediglich aufgerufen werden, damit die Übersicht gewahrt bleibt.
- Bei bewältigung einer Aufgabe sollen die bereits vorhandenen Funktionen verwendet werden.

Erwartete Artefakte (beim Abschluss jeder Aufgabe):
- Skript/Notebook: `H1-561874.py` oder `H1-561874.ipynb`.
- Plots als PNG oder in Notebook eingebettet.
- Kurzer Ergebnisbericht in der H1-561874.md sobald eine Aufgabe abgeschlossen ist.


