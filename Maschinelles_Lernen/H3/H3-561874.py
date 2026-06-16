"""
Hausarbeit 3
Autor: Henry Mirus
Matrikelnummer: 561874
Beschreibung: Regularisierungstechniken (L1/L2 logistische Regression) auf dem
              Breast Cancer Wisconsin Datensatz.
"""

import csv
import gzip
import os
import warnings

import matplotlib.pyplot as plt
import numpy as np
from sklearn.linear_model import LogisticRegressionCV
from sklearn.metrics import accuracy_score
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_PATH = os.path.join(BASE_DIR, 'H3-Wisconsin.csv.gz')
PLOT_DIR = os.path.join(BASE_DIR, 'Plots')
MATRIKELNUMMER = 561874
C_GRID = np.logspace(-3, 3, 61)
EPSILON = 1e-8


def load_wisconsin(path_gz: str) -> tuple[np.ndarray, np.ndarray, list[str]]:
    """Laedt H3-Wisconsin.csv.gz; erste Spalte ist die Diagnose (malignant/benign)."""
    x_list: list[list[float]] = []
    y_list: list[int] = []
    label_map: dict[str, int] = {'malignant': 1, 'benign': 0}

    with gzip.open(path_gz, 'rt', encoding='utf-8') as fh:
        reader = csv.reader(fh)
        header = next(reader)
        feature_names = header[1:]
        for row in reader:
            if not row:
                continue
            y_list.append(label_map[row[0].strip()])
            x_list.append([float(v) for v in row[1:]])

    return np.array(x_list, dtype=np.float64), np.array(y_list, dtype=np.int32), feature_names


def _cv_scores_1d(model: LogisticRegressionCV) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Gibt (log_cs, mean_scores, std_scores) als garantiert 1-D Arrays zurueck.

    scores_ hat in sklearn >=1.9 je nach API die Form
    (n_folds, n_Cs) oder (n_folds, n_Cs, n_l1_ratios).
    """
    class_key = list(model.scores_.keys())[0]
    scores = np.asarray(model.scores_[class_key])
    if scores.ndim == 3:
        scores = scores[:, :, 0]
    return (
        np.log10(np.ravel(model.Cs_)),
        scores.mean(axis=0).ravel(),
        scores.std(axis=0).ravel(),
    )


def _best_c(model: LogisticRegressionCV) -> float:
    return float(np.ravel(model.C_)[0])


def plot_cv_accuracy(
    model: LogisticRegressionCV,
    title: str,
    filename: str,
) -> None:
    """Diagramm 1/2a: Mittlere CV-Genauigkeit als Funktion von log10(C)."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    log_cs, mean_acc, std_acc = _cv_scores_1d(model)
    best = _best_c(model)

    fig, ax = plt.subplots(figsize=(8, 4.5), dpi=120)
    ax.plot(log_cs, mean_acc, color='steelblue', linewidth=2, label='Mittlere CV-Genauigkeit')
    ax.fill_between(log_cs, mean_acc - std_acc, mean_acc + std_acc,
                    color='steelblue', alpha=0.2, label='±1 Std.-Abw.')
    ax.axvline(np.log10(best), color='crimson', linestyle='--', linewidth=1.5,
               label=f'Bestes C = {best:.4f}  (log₁₀ = {np.log10(best):.2f})')
    ax.set_xlabel(r'$\log_{10}(C)$')
    ax.set_ylabel('CV-Genauigkeit')
    ax.set_title(title)
    ax.legend()
    ax.grid(True, alpha=0.3)
    plt.tight_layout()
    out_path = os.path.join(PLOT_DIR, filename)
    plt.savefig(out_path, dpi=120, bbox_inches='tight')
    plt.close(fig)
    print(f'Diagramm gespeichert: {out_path}')


def compute_l1_path(
    x_train: np.ndarray,
    y_train: np.ndarray,
    c_grid: np.ndarray,
) -> np.ndarray:
    """Berechnet L1-Koeffizientenpfad ueber alle C-Werte.

    Verwendet saga mit l1_ratio=1.0 (neue sklearn-API, keine Deprecation-Warnungen).
    Gibt Matrix der Form (len(c_grid), n_features) zurueck.
    """
    from sklearn.linear_model import LogisticRegression
    from sklearn.exceptions import ConvergenceWarning
    path = np.zeros((len(c_grid), x_train.shape[1]), dtype=np.float64)
    with warnings.catch_warnings():
        warnings.filterwarnings('ignore', category=ConvergenceWarning)
        for i, c in enumerate(c_grid):
            m = LogisticRegression(
                l1_ratio=1.0,
                solver='saga',
                C=c,
                max_iter=3000,
                tol=1e-4,
                random_state=MATRIKELNUMMER,
            )
            m.fit(x_train, y_train)
            path[i] = m.coef_.ravel()
    return path


def plot_regularization_path(
    c_grid: np.ndarray,
    path: np.ndarray,
    best_c: float,
    feature_names: list[str],
    filename: str,
) -> None:
    """Diagramm 2b: Koeffizientenwert jedes Merkmals als Funktion von log10(C)."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    log_cs = np.log10(c_grid)

    fig, ax = plt.subplots(figsize=(12, 6), dpi=120)
    for j, name in enumerate(feature_names):
        ax.plot(log_cs, path[:, j], linewidth=1.0, alpha=0.85, label=name)
    ax.axvline(np.log10(best_c), color='black', linestyle='--', linewidth=1.5,
               label=f'Bestes C = {best_c:.4f}')
    ax.axhline(0, color='gray', linewidth=0.5)
    ax.set_xlabel(r'$\log_{10}(C)$')
    ax.set_ylabel('Koeffizientenwert')
    ax.set_title('L1-logistische Regression – Regularisierungspfad')
    ax.legend(fontsize=6, ncol=3, loc='upper left', framealpha=0.7)
    ax.grid(True, alpha=0.3)
    plt.tight_layout()
    out_path = os.path.join(PLOT_DIR, filename)
    plt.savefig(out_path, dpi=120, bbox_inches='tight')
    plt.close(fig)
    print(f'Diagramm gespeichert: {out_path}')


def plot_correlation_matrix(
    x_train_scaled: np.ndarray,
    feature_names: list[str],
    filename: str,
) -> None:
    """Diagramm 3: Korrelationsmatrix (= Kovarianzmatrix der standardisierten Trainingsdaten)."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    n = x_train_scaled.shape[0]
    corr = (x_train_scaled.T @ x_train_scaled) / (n - 1)

    fig, ax = plt.subplots(figsize=(13, 11), dpi=120)
    im = ax.matshow(corr, cmap='RdBu_r', vmin=-1, vmax=1)
    fig.colorbar(im, ax=ax, fraction=0.046, pad=0.04, label='Korrelation')
    ax.set_xticks(range(len(feature_names)))
    ax.set_yticks(range(len(feature_names)))
    ax.set_xticklabels(feature_names, rotation=90, fontsize=6.5)
    ax.set_yticklabels(feature_names, fontsize=6.5)
    ax.set_title('Korrelationsmatrix – Trainingsdatensatz (30 Merkmale)', pad=14)
    plt.tight_layout()
    out_path = os.path.join(PLOT_DIR, filename)
    plt.savefig(out_path, dpi=120, bbox_inches='tight')
    plt.close(fig)
    print(f'Diagramm gespeichert: {out_path}')


def main() -> None:
    """Orchestriert alle geforderten Schritte der Aufgabenstellung."""

    # ── Aufgabe 1a: Datensatz laden ───────────────────────────────────────────
    print('Lade Datensatz ...')
    x, y, feature_names = load_wisconsin(DATA_PATH)
    _, counts = np.unique(y, return_counts=True)
    print(f'Datensatz: {x.shape[0]} Samples, {x.shape[1]} Features')
    print(f'Klassen:   benign={counts[0]}, malignant={counts[1]}')

    x_train_raw, x_test_raw, y_train, y_test = train_test_split(
        x, y, test_size=0.25, random_state=MATRIKELNUMMER, stratify=y,
    )
    print(f'Train: {x_train_raw.shape[0]} Samples  |  Test: {x_test_raw.shape[0]} Samples')

    scaler = StandardScaler()
    x_train = scaler.fit_transform(x_train_raw)
    x_test = scaler.transform(x_test_raw)

    # ── Aufgabe 1: L2-regularisierte logistische Regression ──────────────────
    print('\n--- Aufgabe 1: L2-regularisierte logistische Regression ---')
    model_l2 = LogisticRegressionCV(
        Cs=C_GRID,
        l1_ratios=[0.0],           # reine L2-Regularisierung
        solver='saga',
        cv=5,
        scoring='accuracy',
        max_iter=5000,
        tol=1e-3,
        n_jobs=-1,
        random_state=MATRIKELNUMMER,
        use_legacy_attributes=True,
    )
    model_l2.fit(x_train, y_train)

    best_c_l2 = _best_c(model_l2)
    test_acc_l2 = accuracy_score(y_test, model_l2.predict(x_test))
    print(f'Bestes C (L2):      {best_c_l2:.6f}  (log10 = {np.log10(best_c_l2):.3f})')
    print(f'Testgenauigkeit L2: {test_acc_l2:.4f}  ({test_acc_l2 * 100:.2f} %)')

    plot_cv_accuracy(
        model_l2,
        'L2-logistische Regression – 5-fache Kreuzvalidierung',
        'h3_diagram1_l2_cv_accuracy.png',
    )

    # ── Aufgabe 2: L1-Regularisierung und Merkmalsauswahl ────────────────────
    print('\n--- Aufgabe 2: L1-Regularisierung und Merkmalsauswahl ---')
    model_l1 = LogisticRegressionCV(
        Cs=C_GRID,
        l1_ratios=[1.0],           # reine L1-Regularisierung
        solver='saga',
        cv=5,
        scoring='accuracy',
        max_iter=5000,
        tol=1e-3,
        n_jobs=-1,
        random_state=MATRIKELNUMMER,
        use_legacy_attributes=True,
    )
    model_l1.fit(x_train, y_train)

    best_c_l1 = _best_c(model_l1)
    test_acc_l1 = accuracy_score(y_test, model_l1.predict(x_test))
    coefs_l1 = model_l1.coef_.ravel()
    nonzero_mask = np.abs(coefs_l1) > EPSILON
    nonzero_count = int(nonzero_mask.sum())
    selected_features = [feature_names[i] for i in range(len(feature_names)) if nonzero_mask[i]]

    print(f'Bestes C (L1):            {best_c_l1:.6f}  (log10 = {np.log10(best_c_l1):.3f})')
    print(f'Testgenauigkeit L1:       {test_acc_l1:.4f}  ({test_acc_l1 * 100:.2f} %)')
    print(f'Nicht-Null-Koeffizienten: {nonzero_count} / {len(feature_names)}')
    print('Ausgewaehlte Merkmale:')
    for name in selected_features:
        print(f'  • {name:35s}  w = {coefs_l1[feature_names.index(name)]:+.4f}')

    plot_cv_accuracy(
        model_l1,
        'L1-logistische Regression – 5-fache Kreuzvalidierung',
        'h3_diagram2a_l1_cv_accuracy.png',
    )
    print('Berechne Regularisierungspfad (61 Modelle) ...')
    l1_path = compute_l1_path(x_train, y_train, C_GRID)
    plot_regularization_path(
        C_GRID, l1_path, best_c_l1, feature_names,
        'h3_diagram2b_l1_regularization_path.png',
    )

    # ── Aufgabe 3: Korrelationsmatrix ─────────────────────────────────────────
    print('\n--- Aufgabe 3: Korrelationsmatrix ---')
    plot_correlation_matrix(x_train, feature_names, 'h3_diagram3_correlation_matrix.png')

    # Zusammenfassung fuer den Bericht
    print('\n=== Zusammenfassung ===')
    print(f'{"Modell":<8} {"Bestes C":>12} {"log10(C)":>10} {"Testgenauigkeit":>16}')
    print('-' * 52)
    print(f'{"L2":<8} {best_c_l2:>12.6f} {np.log10(best_c_l2):>10.3f} {test_acc_l2:>16.4f}')
    print(f'{"L1":<8} {best_c_l1:>12.6f} {np.log10(best_c_l1):>10.3f} {test_acc_l1:>16.4f}')
    print(f'L1 Merkmale ({nonzero_count}/{len(feature_names)}): {", ".join(selected_features)}')


if __name__ == '__main__':
    main()
