"""
Hausarbeit 2
Autor: Henry Mirus
Matrikelnummer: 561874
Beschreibung: Klassifikation von 7 Bohnensorten (Dry Beans) mit Ensemble-Methoden.
"""

import csv
import gzip
import os

import matplotlib.pyplot as plt
import numpy as np
from sklearn.ensemble import AdaBoostClassifier, RandomForestClassifier
from sklearn.metrics import ConfusionMatrixDisplay, confusion_matrix
from sklearn.model_selection import GridSearchCV, cross_val_score, train_test_split

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_PATH = os.path.join(BASE_DIR, 'H2-DryBeans.csv.gz')
PLOT_DIR = os.path.join(BASE_DIR, 'Plots')
MATRIKELNUMMER = 561874


def load_dry_beans(path_gz: str) -> tuple[np.ndarray, np.ndarray, list[str], list[str]]:
    """Laedt H2-DryBeans.csv.gz direkt mit gzip und csv; letzte Spalte ist die Klasse."""
    x_list: list[list[float]] = []
    y_list: list[str] = []

    with gzip.open(path_gz, 'rt', encoding='utf-8') as fh:
        reader = csv.reader(fh)
        header = next(reader)
        feature_names = header[:-1]
        for row in reader:
            if not row:
                continue
            x_list.append([float(v) for v in row[:-1]])
            y_list.append(row[-1].strip())

    x = np.array(x_list, dtype=np.float64)
    y = np.array(y_list)
    class_names = sorted(set(y_list))
    return x, y, feature_names, class_names


def print_dataset_overview(x: np.ndarray, y: np.ndarray) -> None:
    """Gibt Shape und Klassenverteilung des gesamten Datensatzes aus (Aufgabe 1b)."""
    classes, counts = np.unique(y, return_counts=True)
    total = y.shape[0]
    print(f'\nDatensatz: {total} Samples, {x.shape[1]} Features, {len(classes)} Klassen')
    print(f'\n{"Klasse":<12} {"Anzahl":>8} {"Anteil":>8}')
    print('-' * 32)
    for cls, cnt in zip(classes, counts):
        print(f'{cls:<12} {cnt:>8d} {cnt / total * 100:>7.2f}%')


def run_cv_model(model, x_train: np.ndarray, y_train: np.ndarray, name: str) -> tuple[float, float]:
    """5-Fold Cross-Validation; gibt mittlere Accuracy und Standardabweichung zurueck."""
    scores = cross_val_score(model, x_train, y_train, cv=5, scoring='accuracy', n_jobs=-1)
    mean_acc = float(scores.mean())
    std_acc = float(scores.std())
    print(f'{name}: CV Accuracy = {mean_acc:.4f} +/- {std_acc:.4f}')
    return mean_acc, std_acc


def run_grid_search(x_train: np.ndarray, y_train: np.ndarray) -> GridSearchCV:
    """GridSearchCV ueber n_estimators x max_depth; fittet das beste RF-Modell."""
    param_grid = {
        'n_estimators': [50, 100, 200],
        'max_depth': [None, 10, 20],
    }
    grid = GridSearchCV(
        RandomForestClassifier(random_state=MATRIKELNUMMER),
        param_grid,
        cv=5,
        scoring='accuracy',
        n_jobs=-1,
        verbose=1,
    )
    grid.fit(x_train, y_train)
    print(f'\nBeste RF-Parameter: {grid.best_params_}')
    print(f'Beste CV-Accuracy:  {grid.best_score_:.4f}')
    return grid


def plot_confusion_matrix(
    y_test: np.ndarray,
    y_pred: np.ndarray,
    class_names: list[str],
    filename: str,
) -> None:
    """Speichert Konfusionsmatrix auf der Testmenge als PNG (Aufgabe 3a)."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    cm = confusion_matrix(y_test, y_pred, labels=class_names)
    fig, ax = plt.subplots(figsize=(9, 7), dpi=120)
    disp = ConfusionMatrixDisplay(cm, display_labels=class_names)
    disp.plot(cmap='Blues', ax=ax, colorbar=False)
    ax.set_title('Konfusionsmatrix - Testmenge (bestes RF-Modell)')
    plt.tight_layout()
    out_path = os.path.join(PLOT_DIR, filename)
    plt.savefig(out_path, dpi=120, bbox_inches='tight')
    plt.close(fig)
    print(f'Konfusionsmatrix gespeichert: {out_path}')


def plot_feature_importances(
    importances: np.ndarray,
    feature_names: list[str],
    filename: str,
    top_n: int = 10,
) -> None:
    """Speichert horizontales Balkendiagramm der Top-N Feature Importances (Aufgabe 3b)."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    order = np.argsort(importances)[::-1][:top_n]
    top_imp = importances[order]
    top_names = [feature_names[i] for i in order]

    fig, ax = plt.subplots(figsize=(8, 5), dpi=120)
    ax.barh(range(top_n), top_imp[::-1], color='steelblue', edgecolor='white')
    ax.set_yticks(range(top_n))
    ax.set_yticklabels(top_names[::-1])
    ax.set_xlabel('Feature Importance (mittlere Impurity-Reduktion)')
    ax.set_title(f'Top-{top_n} Feature Importances - bestes RF-Modell')
    ax.grid(True, axis='x', alpha=0.3)
    plt.tight_layout()
    out_path = os.path.join(PLOT_DIR, filename)
    plt.savefig(out_path, dpi=120, bbox_inches='tight')
    plt.close(fig)
    print(f'Feature Importances gespeichert: {out_path}')


def main() -> None:
    """Orchestriert alle geforderten Schritte der Aufgabenstellung."""

    # Aufgabe 1a: Datensatz laden
    print('Lade Datensatz ...')
    x, y, feature_names, class_names = load_dry_beans(DATA_PATH)

    # Aufgabe 1b: Klassenverteilung
    print_dataset_overview(x, y)

    # Aufgabe 1c: stratifizierter 80/20 Train/Test-Split
    x_train, x_test, y_train, y_test = train_test_split(
        x, y, test_size=0.2, random_state=MATRIKELNUMMER, stratify=y
    )
    print(f'\nTrain: {x_train.shape[0]} Samples  |  Test: {x_test.shape[0]} Samples')

    # Aufgabe 2a: Random Forest (Standardeinstellungen), 5-Fold CV
    print('\n--- Aufgabe 2a: Random Forest (Standardeinstellungen) ---')
    rf_mean, rf_std = run_cv_model(
        RandomForestClassifier(random_state=MATRIKELNUMMER),
        x_train, y_train,
        'Random Forest (Standard)',
    )

    # Aufgabe 2b: AdaBoost (Standardeinstellungen), 5-Fold CV
    print('\n--- Aufgabe 2b: AdaBoost (Standardeinstellungen) ---')
    ada_mean, ada_std = run_cv_model(
        AdaBoostClassifier(random_state=MATRIKELNUMMER),
        x_train, y_train,
        'AdaBoost (Standard)',
    )

    # Aufgabe 2c: GridSearchCV auf Random Forest (9 Kombinationen x 5 Folds)
    print('\n--- Aufgabe 2c: GridSearchCV auf Random Forest ---')
    grid = run_grid_search(x_train, y_train)
    best_rf = grid.best_estimator_

    # Aufgabe 2d: Ergebnistabelle
    print('\n--- Aufgabe 2d: Modellvergleich ---')
    print(f'{"Modell":<35} {"CV Accuracy":>12} {"Std":>8}')
    print('-' * 58)
    print(f'{"Random Forest (Standard)":<35} {rf_mean:>12.4f} {rf_std:>8.4f}')
    print(f'{"AdaBoost (Standard)":<35} {ada_mean:>12.4f} {ada_std:>8.4f}')
    print(f'{"RF getunt (GridSearchCV)":<35} {grid.best_score_:>12.4f} {"(CV)":>8}')
    print(f'Beste RF-Parameter: {grid.best_params_}')

    # Aufgabe 3: Auswertung auf Testmenge
    print('\n--- Aufgabe 3: Analyse auf Testmenge ---')
    y_pred = best_rf.predict(x_test)
    test_acc = float((y_pred == y_test).mean())
    print(f'Test-Accuracy (bestes RF): {test_acc:.4f}')

    # Aufgabe 3a: Konfusionsmatrix
    plot_confusion_matrix(y_test, y_pred, class_names, 'h2_confusion_matrix.png')

    # Aufgabe 3b: Feature Importances (Top 10)
    plot_feature_importances(best_rf.feature_importances_, feature_names, 'h2_feature_importances.png')


if __name__ == '__main__':
    main()
