"""
Hausarbeit 1
Autor: Henry Mirus
Beschreibung: Vollstaendige Umsetzung der H1-FIFA23 Regression in einer ausfuehrbaren Python-Datei.
"""

import csv
import gzip
import os

import matplotlib.pyplot as plt
import numpy as np


# Vorgaben der Aufgabe
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_PATH = os.path.join(BASE_DIR, 'H1-FIFA23.csv.gz')
PLOT_DIR = os.path.join(BASE_DIR, 'Plots')
MATRIKELNUMMER = 561874
FEATURE_COLUMNS = [
    'Age',
    'Height',
    'Weight',
    'International Reputation',
    'Weak Foot',
    'Skill Moves',
    'Value',
    'Wage',
]


def _ensure_unzipped_csv(path_gz: str, prefer_datadir: str = 'Datensätze') -> str:
    """Verwendet bevorzugt vorhandene CSV oder entpackt bei Bedarf die .gz-Datei."""
    if not path_gz.endswith('.gz'):
        raise ValueError('expecting a .gz path')

    base_name = os.path.basename(path_gz[:-3])
    csv_name = base_name if base_name.endswith('.csv') else base_name + '.csv'

    candidates = [
        os.path.join(os.getcwd(), prefer_datadir, csv_name),
        os.path.join(os.path.dirname(path_gz), csv_name),
    ]
    for candidate in candidates:
        if os.path.exists(candidate):
            return candidate

    out_path = os.path.join(os.path.dirname(path_gz), csv_name)
    with gzip.open(path_gz, 'rt', encoding='utf-8') as src, open(out_path, 'wt', encoding='utf-8') as dst:
        for line in src:
            dst.write(line)
    return out_path


def load_csv(path_csv: str, label_col: int = -1, feature_cols: list[int] | None = None, dtype: type = np.float64):
    """Laedt CSV/.csv.gz robust und gibt x, y, feature_names, class_names zurueck."""
    if path_csv.endswith('.gz'):
        try:
            csv_path = _ensure_unzipped_csv(path_csv)
        except Exception:
            csv_path = path_csv
    else:
        csv_path = path_csv

    x_list: list[list[float]] = []
    y_list: list[int] = []
    label_map: dict[str, int] = {}

    with open(csv_path, 'rt', encoding='utf-8') as fh:
        reader = csv.reader(fh)
        header = next(reader)

        n_cols = len(header)
        label_idx = label_col if label_col >= 0 else n_cols + label_col
        feature_indices = [i for i in range(n_cols) if i != label_idx] if feature_cols is None else feature_cols
        feature_names = [header[i] for i in feature_indices]

        for row in reader:
            if not row or len(row) < n_cols:
                continue

            raw_feats = [row[i].strip() for i in feature_indices]
            try:
                feats = [float(v) for v in raw_feats]
            except ValueError:
                try:
                    feats = [float(v.replace(',', '.')) for v in raw_feats]
                except Exception:
                    continue

            label_raw = row[label_idx].strip()
            if label_raw not in label_map:
                label_map[label_raw] = len(label_map)
            y_list.append(label_map[label_raw])
            x_list.append(feats)

    if not x_list:
        raise ValueError(f'Keine verwertbaren Zeilen gefunden in {csv_path}')

    x = np.array(x_list, dtype=dtype)
    y = np.array(y_list, dtype=np.uint16)
    class_names = [None] * len(label_map)
    for name, idx in label_map.items():
        class_names[idx] = name

    return x, y, feature_names, class_names


def train_test_split(
    x: np.ndarray,
    y: np.ndarray,
    train_ratio: float,
    seed: int,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """Notebook-nahe zufaellige Aufteilung in Train/Test."""
    if not (0.0 < train_ratio < 1.0):
        raise ValueError('train_ratio muss zwischen 0 und 1 liegen.')
    if x.shape[0] != y.shape[0]:
        raise ValueError('x und y muessen gleich viele Zeilen haben.')

    generator = np.random.default_rng(seed)
    indices = generator.permutation(x.shape[0])
    split_idx = int(train_ratio * indices.size)
    indices_train = indices[:split_idx]
    indices_test = indices[split_idx:]
    return x[indices_train], y[indices_train], x[indices_test], y[indices_test]


def create_h1_split(path: str, seed: int) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """Set==0 -> Train, Set in {1, 2} -> Val/Test (90/10)."""
    x_all, y_set_encoded, _, class_names = load_csv(
        path_csv=path,
        label_col=0,
        feature_cols=[1, 2, 3, 4, 5, 6, 7, 8, 9],
    )

    y = x_all[:, 0]
    x = x_all[:, 1:]

    set_col = np.array([int(float(class_names[idx])) for idx in y_set_encoded], dtype=np.uint8)
    train_mask = set_col == 0
    holdout_mask = np.isin(set_col, (1, 2))

    x_train, y_train = x[train_mask], y[train_mask]
    x_holdout, y_holdout = x[holdout_mask], y[holdout_mask]
    x_val, y_val, x_test, y_test = train_test_split(x_holdout, y_holdout, train_ratio=0.9, seed=seed)
    return x_train, y_train, x_val, y_val, x_test, y_test


def normalize_features(
    x_train: np.ndarray,
    x_val: np.ndarray,
    x_test: np.ndarray,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """Standardisierung mit Parametern nur aus dem Trainingssatz."""
    mu = x_train.mean(axis=0)
    sigma = x_train.std(axis=0)
    sigma[sigma == 0.0] = 1.0

    x_train_norm = (x_train - mu) / sigma
    x_val_norm = (x_val - mu) / sigma
    x_test_norm = (x_test - mu) / sigma
    return x_train_norm, x_val_norm, x_test_norm


def add_bias_column(x: np.ndarray) -> np.ndarray:
    """Fügt eine Bias-Spalte aus Einsen als erste Spalte hinzu."""
    return np.hstack((np.ones((x.shape[0], 1), dtype=x.dtype), x))


def fit_linear_regression_ols(x: np.ndarray, y: np.ndarray) -> np.ndarray:
    """Stabile analytische OLS-Lösung mit kleiner Regularisierung.

    Nutzt `np.linalg.solve`, um die Berechnung einer expliziten Matrixinverse
    zu vermeiden. Ein kleiner diagonaler Regularisierer wird hinzugefügt, um
    die Konditionierung zu verbessern und numerische Probleme bei nahezu
    singulären Matrizen X^T X zu vermeiden. Fällt auf Pseudo-Inverse zurück,
    falls solve fehlschlägt.
    """
    xtx = x.T @ x
    xty = x.T @ y
    reg = 1e-9
    reg_mat = reg * np.eye(xtx.shape[0], dtype=xtx.dtype)
    try:
        return np.linalg.solve(xtx + reg_mat, xty)
    except np.linalg.LinAlgError:
        return np.linalg.pinv(xtx + reg_mat) @ xty


def mse(y_true: np.ndarray, y_pred: np.ndarray) -> float:
    """Mean Squared Error."""
    return float(np.mean((y_true - y_pred) ** 2))


def r2_score(y_true: np.ndarray, y_pred: np.ndarray) -> float:
    """Bestimmtheitsmaß R^2."""
    sse = np.sum((y_true - y_pred) ** 2)
    sst = np.sum((y_true - np.mean(y_true)) ** 2)
    if sst == 0:
        return 0.0
    return float(1.0 - sse / sst)


def evaluate_model(x_train: np.ndarray, y_train: np.ndarray, x_test: np.ndarray, y_test: np.ndarray, w: np.ndarray) -> dict[str, float]:
    """Berechnet MSE und R^2 auf Train und Test."""
    y_hat_train = x_train @ w
    y_hat_test = x_test @ w
    return {
        'mse_train': mse(y_train, y_hat_train),
        'mse_test': mse(y_test, y_hat_test),
        'r2_train': r2_score(y_train, y_hat_train),
        'r2_test': r2_score(y_test, y_hat_test),
    }


def print_coefficients(feature_names: list[str], w: np.ndarray, titel: str) -> None:
    """Gibt Koeffizienten inkl. kurzer Richtungsauswertung aus."""
    print(f'\n{titel}')
    print(f'Bias: {w[0]:.6f}')
    for name, value in zip(feature_names, w[1:]):
        richtung = 'positiver Zusammenhang' if value >= 0 else 'negativer Zusammenhang'
        print(f'{name:>28s}: {value: .6f}  ({richtung})')


def create_quadratic_features(
    x: np.ndarray | list[np.ndarray] | tuple[np.ndarray, ...],
    feature_names: list[str],
) -> tuple[np.ndarray, list[str]] | tuple[list[np.ndarray], list[str]]:
    """Erweiterung A: Fügt für jedes Merkmal den quadratischen Term x_j^2 hinzu."""
    if isinstance(x, (list, tuple)):
        transformed = [create_quadratic_features(array, feature_names) for array in x]
        if not transformed:
            return [], []
        features = [item[0] for item in transformed]
        names_ext = transformed[0][1]
        return features, names_ext
    x_sq = x ** 2
    x_ext = np.hstack((x, x_sq))
    names_ext = feature_names + [f'{name}^2' for name in feature_names]
    return x_ext, names_ext


def _create_alternative_features_single(x: np.ndarray) -> tuple[np.ndarray, list[str]]:
    """Erweiterung B: 16 alternative Merkmale (8 Basis + 8 Erweiterungen) für ein Array."""
    age = x[:, 0]
    height = x[:, 1]
    weight = x[:, 2]
    reputation = x[:, 3]
    weak_foot = x[:, 4]
    skill_moves = x[:, 5]
    value = x[:, 6]
    wage = x[:, 7]


    # Zweite 8: Quadrate und zusätzliche Interaktionen
    extended_features = np.column_stack(
        (
            age * reputation,
            (age * skill_moves),
            height * skill_moves,
            weight * skill_moves,
            height * weight,
            weak_foot * skill_moves * value,
            np.log1p(value) ** 2,
            np.log1p(wage) ** 2,
        )
    )

    features = np.hstack((x, extended_features))

    names = [
        'Age',
        'Height',
        'Weight',
        'Reputation',
        'WeakFoot',
        'SkillMoves',
        'Value',
        'Wage',
        'Age*Reputation',
        'Age*SkillMoves',
        'Height*SkillMoves',
        'Weight*SkillMoves',
        'Height*Weight',
        'WeakFoot*SkillMoves*Value',
        'log1p(Value)^2',
        'log1p(Wage)^2',
    ]
    return features, names


def create_alternative_features(
    x: np.ndarray | list[np.ndarray] | tuple[np.ndarray, ...],
) -> tuple[np.ndarray, list[str]] | tuple[list[np.ndarray], list[str]]:
    """Erweiterung B für ein Array oder eine Liste/Tuple von Arrays.

    Wenn mehrere Arrays übergeben werden, wird dieselbe Merkmalsbildung auf alle
    Elemente angewendet und die Namen werden nur einmal zurückgegeben.
    """
    if isinstance(x, (list, tuple)):
        transformed = [_create_alternative_features_single(array) for array in x]
        if not transformed:
            return [], []
        features = [item[0] for item in transformed]
        names = transformed[0][1]
        return features, names
    return _create_alternative_features_single(x)


def _prepare_standardized_model(
    x_train_raw: np.ndarray,
    x_val_raw: np.ndarray,
    x_test_raw: np.ndarray,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Standardisiert drei Datensplits mit denselben Trainingsstatistiken."""
    return normalize_features(x_train_raw, x_val_raw, x_test_raw)


def run_step_2_baseline_model(
    x_train: np.ndarray,
    y_train: np.ndarray,
    x_test: np.ndarray,
    y_test: np.ndarray,
    feature_names: list[str],
) -> tuple[dict[str, float], np.ndarray, np.ndarray]:
    """Schritt 2: Basismodell auf standardisierten Features."""
    x_train_b = add_bias_column(x_train)
    x_test_b = add_bias_column(x_test)
    w = fit_linear_regression_ols(x_train_b, y_train)
    eval_result = evaluate_model(x_train_b, y_train, x_test_b, y_test, w)
    print_coefficients(feature_names, w, 'Koeffizienten Basismodell (standardisierte Features)')
    y_hat_test = x_test_b @ w
    plot_model_diagnostics(y_test, y_hat_test, 'Basismodell', 'h1_diagnostics_base_model.png')
    return eval_result, x_train_b, x_test_b


def _run_feature_model_step(
    feature_inputs: list[np.ndarray],
    y_train: np.ndarray,
    y_val: np.ndarray,
    y_test: np.ndarray,
    feature_creator,
    title: str,
    diagnostics_filename: str,
) -> dict[str, object]:
    """Gemeinsame Pipeline für die Feature-Erweiterungen in Schritt 3."""
    features_raw, feature_names = feature_creator(feature_inputs)
    if not isinstance(features_raw, (list, tuple)) or len(features_raw) != 3:
        raise ValueError('feature_creator must return three feature arrays for train/val/test')
    x_train_norm, x_val_norm, x_test_norm = _prepare_standardized_model(
        features_raw[0],
        features_raw[1],
        features_raw[2],
    )
    x_train_b = add_bias_column(x_train_norm)
    x_val_b = add_bias_column(x_val_norm)
    x_test_b = add_bias_column(x_test_norm)
    w = fit_linear_regression_ols(x_train_b, y_train)
    eval_result = evaluate_model(x_train_b, y_train, x_test_b, y_test, w)
    y_hat_test = x_test_b @ w
    plot_model_diagnostics(y_test, y_hat_test, title, diagnostics_filename)
    return {
        'features_raw': features_raw,
        'feature_names': feature_names,
        'x_train_b': x_train_b,
        'x_val_b': x_val_b,
        'x_test_b': x_test_b,
        'w': w,
        'eval_result': eval_result,
        'y_hat_test': y_hat_test,
    }


def run_step_3a_quadratic_model(
    x_train_raw: np.ndarray,
    x_val_raw: np.ndarray,
    x_test_raw: np.ndarray,
    y_train: np.ndarray,
    y_val: np.ndarray,
    y_test: np.ndarray,
    feature_names: list[str],
) -> dict[str, object]:
    """Schritt 3a: quadratische Erweiterung und Basismodell."""
    # deprecated: use `run_step_3_model` for a generic implementation
    return run_step_3_model(
        x_train_raw,
        x_val_raw,
        x_test_raw,
        y_train,
        y_val,
        y_test,
        lambda feature_inputs: create_quadratic_features(feature_inputs, feature_names),
        'Quadratic Model',
        'h1_diagnostics_quadratic_model.png',
    )


def run_step_3b_alternative_model(
    x_train_raw: np.ndarray,
    x_val_raw: np.ndarray,
    x_test_raw: np.ndarray,
    y_train: np.ndarray,
    y_val: np.ndarray,
    y_test: np.ndarray,
) -> dict[str, object]:
    """Schritt 3b: alternative Merkmalskombinationen und Basismodell."""
    # deprecated: use `run_step_3_model` for a generic implementation
    return run_step_3_model(
        x_train_raw,
        x_val_raw,
        x_test_raw,
        y_train,
        y_val,
        y_test,
        create_alternative_features,
        'Alternative Model',
        'h1_diagnostics_alternative_model.png',
    )


def run_step_3_model(
    x_train_raw: np.ndarray,
    x_val_raw: np.ndarray,
    x_test_raw: np.ndarray,
    y_train: np.ndarray,
    y_val: np.ndarray,
    y_test: np.ndarray,
    feature_creator,
    title: str,
    diagnostics_filename: str,
) -> dict[str, object]:
    """Generische Variante für Schritt 3 (Feature-Erweiterung + OLS).

    `feature_creator` muss dieselbe Schnittstelle unterstützen wie vorher
    (Array-Liste -> (features_raw_list, feature_names)).
    """
    return _run_feature_model_step(
        [x_train_raw, x_val_raw, x_test_raw],
        y_train,
        y_val,
        y_test,
        feature_creator,
        title,
        diagnostics_filename,
    )


def run_fss_extension(
    x_train_b: np.ndarray,
    x_val_b: np.ndarray,
    x_test_b: np.ndarray,
    y_train: np.ndarray,
    y_val: np.ndarray,
    y_test: np.ndarray,
    feature_names_with_bias: list[str],
) -> dict[str, object]:
    """Gemeinsame FSS-Erweiterung für beide Feature-Modelle."""
    selected, errors = forward_stepwise_selection(x_train_b, y_train, x_val_b, y_val)
    test_mse = evaluate_fss_path(selected, x_train_b, y_train, x_test_b, y_test)
    val_mse = evaluate_fss_path(selected, x_train_b, y_train, x_val_b, y_val)
    best_k, best_k_mse = get_best_fss_step(val_mse)
    best_names = _format_names_from_selected(selected, feature_names_with_bias, best_k)
    return {
        'selected': selected,
        'errors': errors,
        'test_mse': test_mse,
        'val_mse': val_mse,
        'best_k': best_k,
        'best_k_mse': best_k_mse,
        'best_names': best_names,
    }


def plot_best_feature_combination(
    x_train_b: np.ndarray,
    x_val_b: np.ndarray,
    x_test_b: np.ndarray,
    y_train: np.ndarray,
    y_val: np.ndarray,
    y_test: np.ndarray,
    selected: list[int],
    best_k: int,
    title: str,
    filename: str,
) -> tuple[float, float, float]:
    """Plottet die beste FSS-Feature-Kombination und liefert ihre MSEs zurück."""
    best_features = np.array(selected[: best_k + 1], dtype=np.uint32)
    x_train_best = x_train_b[:, best_features]
    x_val_best = x_val_b[:, best_features]
    x_test_best = x_test_b[:, best_features]
    w_best = fit_linear_regression_ols(x_train_best, y_train)
    y_hat_train_best = x_train_best @ w_best
    y_hat_val_best = x_val_best @ w_best
    y_hat_test_best = x_test_best @ w_best
    train_mse_best = mse(y_train, y_hat_train_best)
    val_mse_best = mse(y_val, y_hat_val_best)
    test_mse_best = mse(y_test, y_hat_test_best)
    plot_model_diagnostics(
        y_val,
        y_hat_val_best,
        title,
        filename,
        y_test,
        y_hat_test_best,
    )
    return train_mse_best, val_mse_best, test_mse_best


# plot_best_feature_combination ist generisch und wird direkt verwendet


def forward_stepwise_selection(
    x_train: np.ndarray,
    y_train: np.ndarray,
    x_val: np.ndarray,
    y_val: np.ndarray,
) -> tuple[list[int], np.ndarray]:
    """Notebook-Methode: Start mit Bias, wähle je Schritt Feature mit bestem Trainings-MSE."""
    selected = [0]
    remaining = list(range(1, x_train.shape[1]))
    errors = np.empty((len(remaining), 2), dtype=np.float64)

    while len(remaining) != 0:
        index_best = remaining[0]
        loss_best = np.inf
        loss_val_best = np.inf

        for index_new_feature in remaining:
            current_features = np.array(selected + [index_new_feature], dtype=np.uint32)
            x_train_current = x_train[:, current_features]
            x_val_current = x_val[:, current_features]

            w = fit_linear_regression_ols(x_train_current, y_train)
            loss_train = mse(y_train, x_train_current @ w)
            loss_val = mse(y_val, x_val_current @ w)

            if loss_train < loss_best:
                index_best = index_new_feature
                loss_best = loss_train
                loss_val_best = loss_val

        errors[len(selected) - 1, 0] = loss_best
        errors[len(selected) - 1, 1] = loss_val_best
        selected.append(index_best)
        remaining.remove(index_best)

    return selected, errors


def evaluate_fss_path(
    selected: list[int],
    x_train: np.ndarray,
    y_train: np.ndarray,
    x_test: np.ndarray,
    y_test: np.ndarray,
) -> np.ndarray:
    """Berechnet Test-MSE entlang der FSS-Pfadlänge (nach jedem Schritt)."""
    steps = len(selected)
    test_mse = np.empty(steps - 1, dtype=np.float64)
    for step in range(1, steps):
        current_features = np.array(selected[: step + 1], dtype=np.uint32)
        x_train_current = x_train[:, current_features]
        x_test_current = x_test[:, current_features]
        w = fit_linear_regression_ols(x_train_current, y_train)
        test_mse[step - 1] = mse(y_test, x_test_current @ w)
    return test_mse


def get_best_fss_step(test_mse: np.ndarray) -> tuple[int, float]:
    """Liefert den besten FSS-Schritt gemäß Test-MSE."""
    best_idx = int(np.argmin(test_mse))
    return best_idx + 1, float(test_mse[best_idx])


def plot_predicted_vs_actual(y_true: np.ndarray, y_pred: np.ndarray, filename: str) -> None:
    """Visualisierung 1: Predicted vs. Actual."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    out_path = os.path.join(PLOT_DIR, filename)

    fig, ax = plt.subplots(figsize=(7, 5), dpi=120)
    ax.scatter(y_true, y_pred, color='steelblue', alpha=0.65, edgecolors='white', s=35, label='Spieler')
    min_v = min(float(np.min(y_true)), float(np.min(y_pred)))
    max_v = max(float(np.max(y_true)), float(np.max(y_pred)))
    ax.plot([min_v, max_v], [min_v, max_v], color='crimson', linewidth=2, label='Ideallinie y=x')
    ax.set_xlabel('Tatsaechliche Overall-Werte')
    ax.set_ylabel('Vorhergesagte Overall-Werte')
    ax.set_title('Predicted vs Actual (Basismodell)')
    ax.grid(True, alpha=0.3)
    ax.legend()
    plt.tight_layout()
    plt.savefig(out_path, dpi=120)
    plt.close(fig)


def plot_split_target_distributions(y_train: np.ndarray, y_val: np.ndarray, y_test: np.ndarray, filename: str) -> None:
    """Visualisierung 0: Zielwert-Verteilung nach Train/Val/Test."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    out_path = os.path.join(PLOT_DIR, filename)

    fig, axes = plt.subplots(1, 3, figsize=(12, 4), dpi=120, sharey=True)
    subsets = [
        ('Train', y_train, '#1F77B4'),
        ('Val', y_val, '#FF7F0E'),
        ('Test', y_test, '#2CA02C'),
    ]
    all_values = np.concatenate((y_train, y_val, y_test))
    x_min = max(1.0, float(np.min(all_values)))
    x_max = float(np.max(all_values))
    tick_start = int(np.floor(x_min / 5.0) * 5)
    tick_end = int(np.ceil(x_max / 5.0) * 5)
    xticks = np.arange(tick_start, tick_end + 1, 5)
    xtick_labels = [str(int(value)) if index % 2 == 0 else '' for index, value in enumerate(xticks)]

    for ax, (title, values, color) in zip(axes, subsets):
        ax.hist(values, bins=20, density=True, color=color, alpha=0.8, edgecolor='white')
        ax.set_xscale('symlog', linthresh=5)
        ax.set_xlim(x_min, x_max)
        ax.set_xticks(xticks)
        ax.set_xticklabels(xtick_labels)
        ax.set_title(f'{title}-Verteilung')
        ax.set_xlabel('Overall-Wert')
        ax.grid(True, alpha=0.25, which='both')

    axes[0].set_ylabel('Dichte')
    fig.suptitle('Zielwert-Verteilung nach dem Split', y=1.02)
    plt.tight_layout()
    plt.savefig(out_path, dpi=120, bbox_inches='tight')
    plt.close(fig)


def plot_feature_overview(x_train_raw: np.ndarray, x_train_norm: np.ndarray, feature_names: list[str], filename: str) -> None:
    """Visualisierung 1: Train-Features vor und nach der Normalisierung."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    out_path = os.path.join(PLOT_DIR, filename)

    indices = list(range(min(4, len(feature_names))))
    fig, axes = plt.subplots(2, 4, figsize=(16, 7), dpi=120, sharey='row')

    for col, feature_idx in enumerate(indices):
        ax_raw = axes[0, col]
        ax_norm = axes[1, col]

        ax_raw.hist(x_train_raw[:, feature_idx], bins=18, color='#1F77B4', alpha=0.8, edgecolor='white')
        ax_raw.set_title(f'{feature_names[feature_idx]}')
        ax_raw.set_xlabel('Rohwert')
        ax_raw.grid(True, alpha=0.25)

        ax_norm.hist(x_train_norm[:, feature_idx], bins=18, color='#FF7F0E', alpha=0.8, edgecolor='white')
        ax_norm.axvline(0.0, color='crimson', linestyle='--', linewidth=1.5)
        ax_norm.set_xlabel('Z-Score')
        ax_norm.grid(True, alpha=0.25)
    axes[0, 0].set_ylabel('Anzahl Spieler')
    axes[1, 0].set_ylabel('Anzahl Spieler')

    # Legende oberhalb der Plots statt seitlicher Beschriftung, um Überlappung zu vermeiden
    import matplotlib.patches as mpatches

    patch_raw = mpatches.Patch(color='#1F77B4', label='Rohdaten')
    patch_norm = mpatches.Patch(color='#FF7F0E', label='Normalisiert')
    fig.legend(handles=[patch_raw, patch_norm], loc='upper center', ncol=2, fontsize=11, frameon=False, bbox_to_anchor=(0.5, 0.97))

    fig.suptitle('Train-Features vor und nach der Normalisierung', y=0.99)
    fig.subplots_adjust(top=0.88)
    plt.tight_layout()
    plt.savefig(out_path, dpi=120, bbox_inches='tight')
    plt.close(fig)


def plot_model_diagnostics(
    y_true: np.ndarray,
    y_pred: np.ndarray,
    title: str,
    filename: str,
    y_true_test: np.ndarray | None = None,
    y_pred_test: np.ndarray | None = None,
) -> None:
    """Visualisierung 2: Vorhersage gegen Realität plus Residuen.

    Standardfall: ein Datensatz (z. B. Test).
    Optional: zweiter Datensatz (z. B. Test), wenn y_true_test/y_pred_test gesetzt sind.
    """
    os.makedirs(PLOT_DIR, exist_ok=True)
    out_path = os.path.join(PLOT_DIR, filename)

    residuals = y_true - y_pred
    has_second_dataset = y_true_test is not None and y_pred_test is not None
    if has_second_dataset:
        residuals_test = y_true_test - y_pred_test

    fig, axes = plt.subplots(1, 2, figsize=(11, 4.8), dpi=120)

    ax = axes[0]
    if has_second_dataset:
        ax.scatter(y_true, y_pred, color='#FF7F0E', alpha=0.65, edgecolors='white', s=35, label='Val-Daten')
        ax.scatter(y_true_test, y_pred_test, color='#2CA02C', alpha=0.65, edgecolors='white', s=35, label='Test-Daten')
        min_v = min(float(np.min(y_true)), float(np.min(y_pred)), float(np.min(y_true_test)), float(np.min(y_pred_test)))
        max_v = max(float(np.max(y_true)), float(np.max(y_pred)), float(np.max(y_true_test)), float(np.max(y_pred_test)))
    else:
        ax.scatter(y_true, y_pred, color='steelblue', alpha=0.65, edgecolors='white', s=35, label='Actual-Werte')
        min_v = min(float(np.min(y_true)), float(np.min(y_pred)))
        max_v = max(float(np.max(y_true)), float(np.max(y_pred)))
    ax.plot([min_v, max_v], [min_v, max_v], color='crimson', linewidth=2, label='Ideallinie y=x')
    ax.set_xlabel('Tatsaechliche Overall-Werte')
    ax.set_ylabel('Vorhergesagte Overall-Werte')
    ax.set_title(f'{title}: Predicted vs Actual')
    ax.grid(True, alpha=0.3)
    ax.legend()

    ax = axes[1]
    if has_second_dataset:
        ax.scatter(y_pred, residuals, color='#FF7F0E', alpha=0.65, edgecolors='white', s=35, label='Residuen (Val)')
        ax.scatter(y_pred_test, residuals_test, color='#2CA02C', alpha=0.65, edgecolors='white', s=35, label='Residuen (Test)')
    else:
        ax.scatter(y_pred, residuals, color='#9467BD', alpha=0.65, edgecolors='white', s=35, label='Residuen')
    ax.axhline(0.0, color='crimson', linewidth=2, label='Null-Linie')
    ax.set_xlabel('Vorhergesagte Overall-Werte')
    ax.set_ylabel('Residuen')
    ax.set_title(f'{title}: Residuen')
    ax.grid(True, alpha=0.3)
    ax.legend()

    plt.tight_layout()
    plt.savefig(out_path, dpi=120, bbox_inches='tight')
    plt.close(fig)


def plot_fss_single_model(errors: np.ndarray, test_mse: np.ndarray, model_name: str, feature_count: int, filename: str) -> None:
    """Visualisierung: MSE-Verlauf waehrend FSS fuer ein einzelnes Modell."""
    os.makedirs(PLOT_DIR, exist_ok=True)
    out_path = os.path.join(PLOT_DIR, filename)

    x_steps = np.arange(1, errors.shape[0] + 1)
    fig, ax = plt.subplots(figsize=(9, 5), dpi=120)

    ax.plot(x_steps, errors[:, 0], 'o--', color='#1F77B4', linewidth=2, markersize=6, label='Train-MSE')
    ax.plot(x_steps, errors[:, 1], 'o-', color='#FF7F0E', linewidth=2, markersize=6, label='Val-MSE')
    ax.plot(x_steps, test_mse, 's-', color='#2CA02C', linewidth=2, markersize=6, label='Test-MSE')
    ax.set_title(f'FSS auf {model_name} ({feature_count} Features)')
    ax.set_xlabel('Anzahl gewaehlter Features')
    ax.set_ylabel('MSE')
    ax.grid(True, alpha=0.3)
    ax.legend(fontsize=10)

    plt.tight_layout()
    plt.savefig(out_path, dpi=120)
    plt.close(fig)


def _format_names_from_selected(selected: list[int], feature_names_with_bias: list[str], count: int) -> list[str]:
    """Hilfsfunktion: Namen der ersten count ausgewaehlten Features (ohne Bias)."""
    idx = selected[1 : count + 1]
    return [feature_names_with_bias[i] for i in idx]


def main() -> None:
    """Orchestriert alle geforderten Schritte der Aufgabenstellung."""
    x_train_raw, y_train, x_val_raw, y_val, x_test_raw, y_test = create_h1_split(
        path=DATA_PATH,
        seed=MATRIKELNUMMER,
    )

    print(f'x_train: {x_train_raw.shape}, y_train: {y_train.shape}')
    print(f'  x_val: {x_val_raw.shape},   y_val: {y_val.shape}')
    print(f' x_test: {x_test_raw.shape},  y_test: {y_test.shape}')

    # Schritt 0: Verteilungen direkt nach dem Split.
    plot_split_target_distributions(y_train, y_val, y_test, 'h1_split_target_distributions.png')

    # Schritt 1: Standardisierung nur auf Basis von x_train.
    x_train, x_val, x_test = normalize_features(x_train_raw, x_val_raw, x_test_raw)
    plot_feature_overview(x_train_raw, x_train, FEATURE_COLUMNS, 'h1_feature_overview_before_after_normalization.png')

    # Schritt 2: Basismodell mit 8 Features.
    eval_b, x_train_b, x_test_b = run_step_2_baseline_model(x_train, y_train, x_test, y_test, FEATURE_COLUMNS)

    # Schritt 3A: Quadratische Erweiterung (16 Features) + OLS.
    quadratic_result = run_step_3_model(
        x_train_raw,
        x_val_raw,
        x_test_raw,
        y_train,
        y_val,
        y_test,
        lambda feature_inputs: create_quadratic_features(feature_inputs, FEATURE_COLUMNS),
        'Quadratic Model',
        'h1_diagnostics_quadratic_model.png',
    )
    x_train_quad_b = quadratic_result['x_train_b']
    x_val_quad_b = quadratic_result['x_val_b']
    x_test_quad_b = quadratic_result['x_test_b']
    eval_quad = quadratic_result['eval_result']
    feature_names_quad = quadratic_result['feature_names']

    # FSS fuer Quadratic Model.
    fss_quad = run_fss_extension(
        x_train_quad_b,
        x_val_quad_b,
        x_test_quad_b,
        y_train,
        y_val,
        y_test,
        ['Bias'] + feature_names_quad,
    )
    selected_quad = fss_quad['selected']
    errors_quad = fss_quad['errors']
    test_mse_quad = fss_quad['test_mse']
    val_mse_quad = fss_quad['val_mse']
    best_quad_k = fss_quad['best_k']
    best_quad_names = fss_quad['best_names']

    # Schritt 3B: Alternative 16 Features + FSS.
    alternative_result = run_step_3_model(
        x_train_raw,
        x_val_raw,
        x_test_raw,
        y_train,
        y_val,
        y_test,
        create_alternative_features,
        'Alternative Model',
        'h1_diagnostics_alternative_model.png',
    )
    x_train_alt_b = alternative_result['x_train_b']
    x_val_alt_b = alternative_result['x_val_b']
    x_test_alt_b = alternative_result['x_test_b']
    eval_alt = alternative_result['eval_result']
    feature_names_alt = alternative_result['feature_names']

    fss_alt = run_fss_extension(
        x_train_alt_b,
        x_val_alt_b,
        x_test_alt_b,
        y_train,
        y_val,
        y_test,
        ['Bias'] + feature_names_alt,
    )
    selected_alt = fss_alt['selected']
    errors_alt = fss_alt['errors']
    test_mse_alt = fss_alt['test_mse']
    val_mse_alt = fss_alt['val_mse']
    best_alt_k = fss_alt['best_k']
    best_alt_names = fss_alt['best_names']

    # Schritt 4: Visualisierungen.
    plot_fss_single_model(errors_quad, test_mse_quad, 'quadratic_model', 16, 'h1_fss_mse-change_quadratic_model.png')
    plot_fss_single_model(errors_alt, test_mse_alt, 'alternative_model', 16, 'h1_fss_mse-change_alternative_model.png')

    # Beste Feature-Kombinationen pro Modell plotten.
    train_mse_best_quad, val_mse_best_quad, test_mse_best_quad = plot_best_feature_combination(
        x_train_quad_b,
        x_val_quad_b,
        x_test_quad_b,
        y_train,
        y_val,
        y_test,
        selected_quad,
        best_quad_k,
        f'Best quadratic_model ({best_quad_k} Features)',
        'h1_fss_best_quadratic_model.png',
    )

    train_mse_best_alt, val_mse_best_alt, test_mse_best_alt = plot_best_feature_combination(
        x_train_alt_b,
        x_val_alt_b,
        x_test_alt_b,
        y_train,
        y_val,
        y_test,
        selected_alt,
        best_alt_k,
        f'Best alternative_model ({best_alt_k} Features)',
        'h1_fss_best_alternative_model.png',
    )

    print('\nMetriken Basismodell')
    print(eval_b)
    print('\nMetriken quadratic_model')
    print(eval_quad)
    print('\nMetriken alternative_model')
    print(eval_alt)
    print(f"\nBestes FSS-Quadratisches-Modell: {best_quad_k} Features: {', '.join(best_quad_names)}")
    print(f"Train-MSE={train_mse_best_quad:.4f}, Val-MSE={val_mse_best_quad:.4f}, Test-MSE={test_mse_best_quad:.4f}")
    print(f"\nBestes FSS-Alternatives-Modell: {best_alt_k} Features: {', '.join(best_alt_names)}")
    print(f"Train-MSE={train_mse_best_alt:.4f}, Val-MSE={val_mse_best_alt:.4f}, Test-MSE={test_mse_best_alt:.4f}")


if __name__ == '__main__':
    main()

