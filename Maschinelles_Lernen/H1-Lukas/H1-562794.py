import numpy as np
import matplotlib.pyplot as plt
import gzip


def load_data(file_csv: str) -> tuple[np.ndarray, np.ndarray]:
    """
    Lädt einen Datensatz.
    :param file_csv: Pfad zur CSV-Datei
    :return: Tupel `(x_train, y_train, x_val, y_val)`, wobei:
             `x_train` ein Numpy-Array der Merkmale ist, bei dem Set = 0 ist;
             `y_train` ein Numpy-Array der Label ist, bei dem Set = 0 ist;
             `x_val` ein Numpy-Array der Merkmale , bei dem Set = 1 ist;
             `y_val` ein Numpy-Array der Label, bei dem Set = 1 ist;
    """
    header = []
    delimiter = ","

    data_val = []
    data_train = []

    open_func = gzip.open if file_csv.endswith(".gz") else gzip.open
    with open_func(file_csv, "rt", encoding="utf-8") as file_handle:
        for index_line, line in enumerate(file_handle):
            line = line.strip()
            if index_line == 0:
                header = line.split(delimiter)

            elif line != "":
                values = line.split(delimiter)
                if len(values) != len(header):
                    raise ValueError(
                        f"Unerwartete Spalenzahl in Zeile {index_line + 1}"
                    )
                if int(values[0]) == 0:
                    data_train.append([float(x) for x in values[1:]])
                elif int(values[0]) == 1 or int(values[0]) == 2:
                    data_val.append([float(x) for x in values[1:]])

    # Listen mit Zahlen zu Numpy-Arrays konvertieren
    train_set = np.array(data_train, dtype=np.float64)
    y_train = train_set[:, 0]
    x_train = train_set[:, 1:]

    val_set = np.array(data_val, dtype=np.float64)
    y_val = val_set[:, 0]
    x_val = val_set[:, 1:]
    return x_train, y_train, x_val, y_val


def val_test_split(
    x: np.ndarray, y: np.ndarray, train_ratio: float
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """
    Teilt einen Datensatz in einen Validierungs- und einen Testsatz auf.

    :param x: Eingabewerte als Numpy-Array der Form `(n, p)`, wobei
              `n` die Anzahl an Beobachtungen ist und `p` - die Anzahl an Merkmalen.
    :param x: Eingabewerte als Numpy-Array der Form `(n, )`, wobei
              `n` die Anzahl der Label ist
    :param train_ratio: Anteil der Beobachtungen zur Bildung des Trainingssatzes.
    :return: Tupel `(x_train, y_train, x_test, y_test)`.
    """
    # Elemente mischen
    generator = np.random.default_rng(561874)
    indices = generator.permutation(x.shape[0])

    # Trainings- und Testsatz erzeugen
    train_size = int(train_ratio * indices.size)

    indices_val = indices[:train_size]
    indices_test = indices[train_size:]

    x_val = x[indices_val, :]
    y_val = y[indices_val]

    x_test = x[indices_test, :]
    y_test = y[indices_test]

    return x_val, y_val, x_test, y_test


def normalize_features(
    x: np.ndarray,
) -> tuple[np.ndarray]:
    """
    Z-Score-Normalisierung.

    :param x: Trainingssatz mit normierten Merkmale als Numpy-Array der Form `(n, p)`, wobei
                    `n` die Anzahl an Beobachtungen ist und `p` - die Anzahl an Merkmalen. Spalte
                    `0` muss die Bias-Spalte sein.
    :return: Tupel `(x_norm,  mu, sigma)`.
    """
    mu = x.mean(axis=0)
    sigma = x.std(axis=0)
    sigma[sigma == 0] = 1
    x_norm = (x - mu) / sigma

    return x_norm, mu, sigma


def add_bias(x: np.ndarray) -> np.ndarray:
    """
    Fügt Bias-Spalte zur Merkmal-Matrix hinzu.
    :param x: Merkmalsmatrix mit `n` Beobachtungen als Array der Form `(n, p)`.
    :return: Array der Form `(n, p + 1)` mit Bias-Spalte (Einsen) vorne.
    """
    bias = np.ones((x.shape[0], 1), dtype=x.dtype)
    return np.hstack((bias, x))


def analytical_solution(x: np.ndarray, y: np.ndarray):
    """
    Berechnet die OLS-Lösung für lineare Regression: w = (X^T X)^{-1} X^T y
    Mit Regularisierung, um Singularität zu vermeiden.

    :param x: Feature-Matrix (z. B. (n, p)).
    :param y: Label-Vektor (z. B. (n,)).
    :return: Gewichtsvektor w (z. B. (p,)).
    """
    X = x
    lambda_reg = 1e-1
    w = np.linalg.solve(X.T @ X + lambda_reg * np.eye(X.shape[1]), X.T @ y)

    return w


def predict(x, w):
    """
    Berechnet die Vorhersage des linearen Modells: ŷ = X @ w

    :param dataset: Dataset mit Labels in Spalte 0, Bias in 1, Features in 2+.
    :param w: Gewichtsvektor (Form (p,)).
    :return: Vorhersagen als Array der Form (n,).
    """
    X = x
    return X @ w  # Matrixmultiplikation


def mse_loss(x: np.ndarray, y: np.ndarray, w):
    """
    Berechnet den mittleren quadratischen Fehler (MSE) des Modells:

    :param x: Feature-Vektor als Array der Form `(n,)`.
    :param y: Label-Vektor als Array der Form `(n,)`.
    :param w: Gewichte.
    :return: MSE-Verlust.
    """
    y_pred = predict(x, w)
    residuals = y - y_pred
    return np.mean(residuals**2)


def create_squared_features(x: np.ndarray) -> np.ndarray:
    """
    Erzeugt eine Matrix mit squared Features.

    :param x: Eingabewerte für Merkmale als Numpy-Array der Form `(n, p)`.
    :return: Array der Form `(n, 2p)` mit den Werten der neuen Features.
    """
    n, p = x.shape
    result = np.empty((n, 2 * p), dtype=x.dtype)

    for i in range(p):
        result[:, i] = x[:, i]  # lineare Features
        result[:, p + i] = x[:, i] ** 2

    return result


def create_new_features(x: np.ndarray) -> np.ndarray:
    """
    Erzeugt eine Matrix mit neuen Features. Für k Merkmale wird merkmal x0 mit xk mutipliziert, x1 mit xk-1 etc.

    :param x: Eingabewerte für Merkmale als Numpy-Array der Form `(n, p)`.
    :return: Array der Form `(n, 2p)` mit den Werten der neuen Features.
    """
    n, p = x.shape
    result = np.empty((n, 2 * p), dtype=x.dtype)

    for i in range(p):
        result[:, i] = x[:, i]  # lineare Features
        result[:, p + i] = x[:, i] * x[:, p - i - 1]

    return result


def r_squared(y_true: np.ndarray, y_pred: np.ndarray) -> float:
    """
    Berechnet das Bestimmtheitsmaß R².

    :param y_true: Echte Labels.
    :param y_pred: Vorhersagen des Modells.
    :return: R² (zwischen 0 und 1, oder negativ).
    """
    ss_res = np.sum((y_true - y_pred) ** 2)  # Sum of Squared Residuals
    ss_tot = np.sum((y_true - np.mean(y_true)) ** 2)  # Total Sum of Squares
    return 1 - (ss_res / ss_tot)


def forward_stepwise_selection(
    x_train: np.ndarray, y_train: np.ndarray, x_val: np.ndarray, y_val: np.ndarray
):
    """
    Vorwärtsgerichtete schrittweise Auswahl.

    :param x_train: Trainingssatz mit normierten Merkmale als Numpy-Array der Form `(n, p)`, wobei
                    `n` die Anzahl an Beobachtungen ist und `p` - die Anzahl an Merkmalen. Spalte
                    `0` muss die Bias-Spalte sein.
    :param y_train: Ausgabewerte für die Beobachtungen im Trainingssatz.
    :param x_val: Validierungssatz mit normierten Merkmale als Numpy-Array der Form `(m, p)`, wobei
                  `m` die Anzahl an Beobachtungen ist und `p` - die Anzahl an Merkmalen. Spalte `0`
                  muss die Bias-Spalte sein.
    :param y_val: Ausgabewerte für die Beobachtungen im Validierungssatz.
    :return: Tupel `(selected, errors)`. Hier ist:
             `selected` eine `list` der Länge `p`, die die Indizes der schrittweise ausgewählten
             Features beinhaltet. Das erste Element ist `0`;
             `errors` eine Numpy-Array der Form `(p - 1, 2)`, die die Trainings- und
             Validierungsverluste aller Modelle beinhaltet.
    """
    selected = [0]  # Indizes der gewählten Features
    remaining = list(
        range(1, x_train.shape[1])
    )  # Indizes der noch verfügbaren Features
    errors = np.empty((len(remaining), 2), dtype=np.float64)

    while len(remaining) != 0:
        index_best, loss_best = 0, 0.0
        for i, index_new_feature in enumerate(remaining):

            current_features = np.array(selected + [index_new_feature], dtype=np.uint32)
            x_train_current = x_train[:, current_features]
            x_val_current = x_val[:, current_features]

            w = analytical_solution(x_train_current, y_train)

            loss_train = mse_loss(x_train_current, y_train, w)
            loss_val = mse_loss(x_val_current, y_val, w)

            if i == 0 or loss_val < loss_best:
                index_best, loss_best = index_new_feature, loss_val
                errors[len(selected) - 1, 0] = loss_train
                errors[len(selected) - 1, 1] = loss_val

        # Das Feature, das zur größten Verbesserung führt, hinzufügen
        selected.append(index_best)
        remaining.remove(index_best)
        del index_best, loss_best, i, index_new_feature

    return selected, errors


def plot_train_validation(
    errors: np.ndarray,
    xlabel: str,
    model_labels: list[str],
    ax,
    title: str = "Figure",
):
    """
    Visualisiert Trainings- und Validierungsverluste
    in einem gegebenen Subplot.
    """

    xs = np.arange(errors.shape[0])

    ax.plot(
        xs,
        errors[:, 0],
        "o--",
        c="#1F77B4",
        label="Training",
    )

    ax.plot(
        xs,
        errors[:, 1],
        "o-",
        c="#FF7F0E",
        label="Validierung",
    )

    ax.set(
        axisbelow=True,
        xlim=[xs[0] - 0.2, xs[-1] + 0.2],
        xticks=xs,
        xticklabels=model_labels,
    )

    ax.set_xlabel(xlabel)
    ax.set_ylabel("Verlustwert")
    ax.set_title(title)

    ax.grid(
        color="#A0A0A0",
        linestyle="--",
        linewidth=0.5,
    )

    ax.legend()


def plot_predicted_vs_actual(
    y: np.ndarray,
    x: np.ndarray,
    w: np.ndarray,
    ax,
    title: str = "Figure",
):
    """
    Streudiagramm der tatsächlichen vs.
    vorhergesagten Werte.
    """

    y = y.ravel()
    y_pred = predict(x, w).ravel()

    ax.scatter(
        y,
        y_pred,
        alpha=0.7,
        label="Vorhersagen",
    )

    min_val = min(np.min(y), np.min(y_pred))
    max_val = max(np.max(y), np.max(y_pred))

    ax.plot(
        [min_val, max_val],
        [min_val, max_val],
        color="red",
        linestyle="--",
        label="Ideale Vorhersage",
    )

    ax.set_xlabel("Tatsächliche Werte")
    ax.set_ylabel("Vorhergesagte Werte")
    ax.set_title(title)

    ax.grid(True)
    ax.legend()


np.set_printoptions(suppress=True, precision=2)

# Bitte Pfad zur CSV-Datei anpassen
file_dataset = "H1-FIFA23.csv.gz"

# Datei Laden und in trainings und validierungsdaten aufteilen
x_train, y_train, x_val, y_val = load_data(file_dataset)

# Validierungsdaten in Validierungsdaten (90%) und Testdaten (10%) aufteilen
x_val, y_val, x_test, y_test = val_test_split(x_val, y_val, 0.9)

# Normierung nach den Werten der Trainingsdaten
x_train_norm, mu, sigma = normalize_features(x_train)
x_test_norm = (x_test - mu) / sigma
x_val_norm = (x_val - mu) / sigma

# Bias hinzufügen
x_train_norm_b = add_bias(x_train_norm)
x_test_norm_b = add_bias(x_test_norm)
x_val_norm_b = add_bias(x_val_norm)

# Analytische lösung ausrechnen
w = analytical_solution(x_train_norm_b, y_train)

# MSE der verschiedenen Datensätze berechnen
mse_train = mse_loss(x_train_norm_b, y_train, w)
mse_test = mse_loss(x_test_norm_b, y_test, w)

# Bestimmtheitsmaß R² berechnen
r_squared_train = r_squared(y_train, predict((x_train_norm_b), w))
r_squared_test = r_squared(y_test, predict((x_test_norm_b), w))

# Ausgabe
print(
    f"R² von den Trainingsdaten: {r_squared_train} und den Testdaten: {r_squared_test}"
)
print(f"MSE von den Trainingsdaten: {mse_train} und den Testdaten: {mse_test} \n")
# FSS auf quadratischen Merkmalen
selected, errors = forward_stepwise_selection(
    x_train_norm_b, y_train, x_val_norm_b, y_val
)

clabels = [str(i + 1) for i in range(errors.shape[0])]

# Quadratische Merkmale hinzufügen
x_train_sq = create_squared_features(x_train_norm)
x_test_sq = create_squared_features(x_test_norm)
x_val_sq = create_squared_features(x_val_norm)

# Normalisierung der quadratischen Merkmalen ohne Bias
x_train_sq_norm, mu_sq, sigma_sq = normalize_features(x_train_sq)  # Ohne Bias!
x_train_sq_norm = add_bias(x_train_sq_norm)
x_test_sq_norm = add_bias((x_test_sq - mu_sq) / sigma_sq)
x_val_sq_norm = add_bias((x_val_sq - mu_sq) / sigma_sq)

# Analytische Lösung der quadratischen Merkmale
w_sq = analytical_solution(x_train_sq_norm, y_train)

# MSE der verschiedenen Datensätze berrechnen
mse_train_sq = mse_loss(x_train_sq_norm, y_train, w_sq)
mse_test_sq = mse_loss(x_test_sq_norm, y_test, w_sq)

# Bestimmtheitsmaß R² berechnen
r_squared_train_sq = r_squared(y_train, predict(x_train_sq_norm, w_sq))
r_squared_test_sq = r_squared(y_test, predict(x_test_sq_norm, w_sq))

# Ausgabe
print(
    f"R² von den Trainingsdaten: {r_squared_train_sq} und den Testdaten: {r_squared_test_sq} mit quadrierten Merkmalen"
)
print(
    f"MSE von den Trainingsdaten: {mse_train_sq} und den Testdaten: {mse_test_sq} mit quadrierten Merkmalen \n"
)

# FSS auf quadratischen Merkmalen
selected, errors_sq = forward_stepwise_selection(
    x_train_sq_norm, y_train, x_val_sq_norm, y_val
)
clabels_sq = [str(i + 1) for i in range(errors_sq.shape[0])]

# Datensatz erweitert mit neuen 8 Merkmalen
x_train_new = create_new_features(x_train_norm)
x_test_new = create_new_features(x_test_norm)
x_val_new = create_new_features(x_val_norm)

# Normierung
x_train_new_norm, mu_new, sigma_new = normalize_features(x_train_new)  # Ohne Bias!
x_train_new_norm = add_bias(x_train_new_norm)
x_test_new_norm = add_bias((x_test_new - mu_new) / sigma_new)
x_val_new_norm = add_bias((x_val_new - mu_new) / sigma_new)

# Analytische Lösung berechnen
w_new = analytical_solution(x_train_new_norm, y_train)

# MSE der verschiedenen Datensätze berrechnen
mse_train_new = mse_loss(x_train_new_norm, y_train, w_new)
mse_test_new = mse_loss(x_test_new_norm, y_test, w_new)

# Bestimmtheitsmaß R² berechnen
r_squared_train_new = r_squared(y_train, predict(x_train_new_norm, w_new))
r_squared_test_new = r_squared(y_test, predict(x_test_new_norm, w_new))

# Ausgabe
print(
    f"R² von den Trainingsdaten: {r_squared_train_new} und den Testdaten: {r_squared_test_new} mit neuen Merkmalen"
)
print(
    f"MSE von den Trainingsdaten: {mse_train_new} und den Testdaten: {mse_test_new} mit neuen Merkmalen \n"
)

# FSS auf neuen Merkmalen
selected, errors_new = forward_stepwise_selection(
    x_train_new_norm, y_train, x_val_new_norm, y_val
)

clabels_new = [str(i + 1) for i in range(errors_new.shape[0])]

# Plot y vs. y_pred von allen Drei Modellen
fig, axes = plt.subplots(1, 3, figsize=(18, 6))

plot_predicted_vs_actual(
    y_test,
    x_test_norm_b,
    w,
    axes[0],
    title="Lineares Modell",
)

plot_predicted_vs_actual(
    y_test,
    x_test_sq_norm,
    w_sq,
    axes[1],
    title="Quadratische Features",
)

plot_predicted_vs_actual(
    y_test,
    x_test_new_norm,
    w_new,
    axes[2],
    title="Neue Features",
)

plt.tight_layout()
plt.show()

# Trian- vs Validationerror von allen drei Modellen
fig, axes = plt.subplots(1, 3, figsize=(18, 6))

labels = ["Linear", "Polynom Grad 2", "Polynom Grad 3"]

plot_train_validation(
    errors,
    "Modell",
    clabels,
    axes[0],
    title="Linear",
)

plot_train_validation(
    errors_sq,
    "Modell",
    clabels_sq,
    axes[1],
    title="Quadratische Merkamle",
)

plot_train_validation(
    errors_new,
    "Modell",
    clabels_new,
    axes[2],
    title="Neue Merkmale",
)

plt.tight_layout()
plt.show()
