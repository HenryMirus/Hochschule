#include "hanoiwidget.h"

#include <QFont>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

namespace
{
// Smoothstep-Beschleunigungsfunktion f(t) = t²·(3 − 2t), t ∈ [0,1].
// f'(0) = f'(1) = 0  →  harmonischer Übergang ohne Geschwindigkeitssprung.
// Die Zugdauer in HanoiEinstellungen skaliert diese Funktion zeitlich.
inline qreal smoothstep(qreal t)
{
    t = qBound(0.0, t, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

// Kubische Bézier-Interpolation für eine einzelne Koordinate.
// Eingabe: t – Parameter [0,1]; p0..p3 – Stützpunkte
// Rückgabe: Koordinatenwert bei Parameter t
inline qreal bezier1D(qreal t, qreal p0, qreal p1, qreal p2, qreal p3)
{
    const qreal u = 1.0 - t;
    return u*u*u*p0 + 3.0*u*u*t*p1 + 3.0*u*t*t*p2 + t*t*t*p3;
}
} // namespace

/*
 * Autor: Henry Mirus
 * Eingabe: parent – übergeordnetes Widget
 * Rückgabe: –
 * Seiteneffekte: Richtet den Animationstimer ein (16 ms ≈ 60 FPS),
 *                initialisiert das Spiel mit 5 Scheiben / 800 ms/Zug.
 */
HanoiWidget::HanoiWidget(QWidget *parent)
    : QWidget(parent)
{
    m_animTimer = new QTimer(this);
    m_animTimer->setInterval(16); // ~60 FPS
    connect(m_animTimer, &QTimer::timeout, this, &HanoiWidget::onAnimTimer);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(minimumSizeHint());

    m_spiel.initialisiere(m_einstellungen.scheibenzahl);
    initialisiereAnimationszustand();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Empfohlene Widget-Größe (von Scheibenanzahl abhängig)
 * Seiteneffekte: –
 */
QSize HanoiWidget::sizeHint() const
{
    const int n = qMax(1, m_einstellungen.scheibenzahl);
    const int sectionW = n * 22 + 70;
    return QSize(3 * sectionW + 40, n * 26 + 140);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Absolute Mindestgröße (400 × 300 Pixel)
 * Seiteneffekte: –
 */
QSize HanoiWidget::minimumSizeHint() const
{
    return QSize(400, 300);
}

/*
 * Autor: Henry Mirus
 * Eingabe: e – neue Spieleinstellungen
 * Rückgabe: –
 * Seiteneffekte: Stoppt den Animationstimer, initialisiert HanoiSpiel neu,
 *                setzt m_animPegs auf den Anfangszustand, sendet
 *                zustandGeaendert(Bereit) und zugFortschrittGeaendert(0, …).
 */
void HanoiWidget::neuesSpiel(const HanoiEinstellungen &e)
{
    m_animTimer->stop();
    m_einstellungen = e;

    m_spiel.initialisiere(e.scheibenzahl);
    initialisiereAnimationszustand();

    m_t = 0.0;
    m_zugElapsedBeforeMs = 0;
    m_zustand = AnimationsZustand::Bereit;

    setMinimumSize(minimumSizeHint());
    update();

    emit zustandGeaendert(m_zustand);
    emit zugFortschrittGeaendert(0, m_spiel.gesamtzuege());
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Startet die Animation (nur im Zustand Bereit wirksam).
 *                Setzt m_t und den Elapsed-Zähler zurück, startet Animationstimer.
 */
void HanoiWidget::starteAnimation()
{
    if (m_zustand != AnimationsZustand::Bereit) {
        return;
    }

    m_t = 0.0;
    m_zugElapsedBeforeMs = 0;
    m_zustand = AnimationsZustand::Laeuft;
    m_zugTimer.start();
    m_animTimer->start(16);

    emit zustandGeaendert(m_zustand);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Bricht die laufende oder pausierte Animation ab. Konstruiert
 *                den Endzustand direkt aus der Scheibenliste (alle Scheiben auf
 *                Pflock 2), ohne jeden einzelnen Zug auszuführen. Sendet
 *                zugFortschrittGeaendert und zustandGeaendert(Beendet).
 */
void HanoiWidget::beendeAnimation()
{
    if (m_zustand == AnimationsZustand::Bereit
            || m_zustand == AnimationsZustand::Beendet) {
        return;
    }

    m_animTimer->stop();

    // Endzustand direkt konstruieren: alle Scheiben auf Pflock 2
    for (int i = 0; i < 3; ++i) {
        m_animPegs[i].leere();
    }
    for (const Scheibe &s : m_spiel.scheibenliste()) {
        m_animPegs[2].schiebeDrauf(s);
    }

    m_t = 0.0;
    m_zustand = AnimationsZustand::Beendet;
    update();

    emit zugFortschrittGeaendert(m_spiel.gesamtzuege(), m_spiel.gesamtzuege());
    emit zustandGeaendert(m_zustand);
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Wechselt zwischen Laeuft und Pausiert.
 *                Beim Pausieren: Elapsed akkumulieren, Timer stoppen.
 *                Beim Fortsetzen: neuen Elapsed-Timer starten, Animationstimer neu.
 */
void HanoiWidget::pauseWechsel()
{
    if (m_zustand == AnimationsZustand::Laeuft) {
        m_zugElapsedBeforeMs += m_zugTimer.elapsed();
        m_zugTimer.invalidate();
        m_animTimer->stop();
        m_zustand = AnimationsZustand::Pausiert;
        emit zustandGeaendert(m_zustand);

    } else if (m_zustand == AnimationsZustand::Pausiert) {
        m_zugTimer.start();
        m_animTimer->start(16);
        m_zustand = AnimationsZustand::Laeuft;
        emit zustandGeaendert(m_zustand);
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: event – Paint-Ereignis
 * Rückgabe: –
 * Seiteneffekte: Zeichnet vollständig neu:
 *                1. Dunkler Hintergrund
 *                2. Braune Basisplatte
 *                3. Drei Stäbe
 *                4. Statische Scheiben auf den Pflöcken
 *                5. Animierende Scheibe auf der Bézier-Bahn (falls Laeuft)
 *                6. Pflock-Beschriftungen
 */
void HanoiWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int W = width();
    const int H = height();
    const int n = qMax(1, m_spiel.scheibenzahl());

    // --- Layout-Konstanten ---
    const int bottomMargin = 28;
    const int topMargin    = 10;
    const int sideMargin   = 12;
    const int availW       = W - 2 * sideMargin;
    const int sectionW     = availW / 3;

    // Scheibenhöhe skaliert mit Widget-Höhe und Scheibenanzahl
    const int diskH = qMax(14, qMin(32, (H - bottomMargin - topMargin - 80) / (n + 2)));

    // Scheibenbreiten (min → kleinste, max → größte)
    const int maxDiskW = sectionW - 16;
    const int minDiskW = qMax(diskH + 6, maxDiskW / (n + 1));

    // Pflock-Mittelpunkte (X)
    const int pegX[3] = {
        sideMargin + sectionW / 2,
        sideMargin + sectionW + sectionW / 2,
        sideMargin + 2 * sectionW + sectionW / 2
    };

    const int baseY   = H - bottomMargin;      // Oberkante der Basisplatte
    const int pegH    = n * diskH + 20;         // Stabhöhe
    const int pegTopY = baseY - pegH;           // Oberkante der Stäbe
    const int rodW    = 8;                      // Stabbreite

    // Mitte der Scheiben-Y-Koordinate für Stapelposition j (0 = unterste)
    auto diskCenterY = [&](int stackPos) -> qreal {
        return qreal(baseY) - stackPos * diskH - diskH / 2.0;
    };

    // Scheibenbreite für Durchmesser d
    auto diskWidth = [&](int d) -> int {
        if (n <= 1) return maxDiskW;
        return minDiskW + (maxDiskW - minDiskW) * (d - 1) / (n - 1);
    };

    // Scheitelpunkt-Y der Bézier-Bahn (Scheibenmitte im Bogen)
    const qreal clearanceCenterY =
        qreal(qMax(topMargin + diskH / 2,
                   pegTopY - 35 - diskH / 2));

    // === 1. Hintergrund ===
    p.fillRect(rect(), QColor(28, 28, 38));

    // === 2. Basisplatte ===
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(95, 68, 42));
    p.drawRoundedRect(sideMargin, baseY, availW, 10, 3, 3);

    // === 3. Stäbe ===
    p.setBrush(QColor(148, 110, 68));
    for (int i = 0; i < 3; ++i) {
        p.drawRoundedRect(pegX[i] - rodW / 2, pegTopY, rodW, pegH, 2, 2);
    }

    // Welcher Pflock liefert die gerade animierende Scheibe?
    const bool animating = (m_zustand == AnimationsZustand::Laeuft)
                           && m_spiel.hatWeitereZuege();
    const int  animVon   = animating ? m_spiel.currentMoveVon() : -1;

    // === 4. Statische Scheiben ===
    for (int pegIdx = 0; pegIdx < 3; ++pegIdx) {
        const QVector<Scheibe> &scheiben = m_animPegs[pegIdx].scheiben();
        // Oberste Scheibe des Quellpflocks wird animiert → überspringen
        const int endI = (pegIdx == animVon) ? scheiben.size() - 1 : scheiben.size();

        for (int j = 0; j < endI; ++j) {
            const Scheibe &s  = scheiben[j];
            const int dw      = diskWidth(s.durchmesser());
            const int dx      = pegX[pegIdx] - dw / 2;
            const int dy      = qRound(diskCenterY(j)) - diskH / 2;

            p.setBrush(s.farbe());
            p.setPen(QPen(s.farbe().darker(170), 1));
            p.drawRoundedRect(dx, dy, dw, diskH - 2, 5, 5);

            // Durchmesser als Zahl in der Scheibenmitte anzeigen
            p.setPen(QColor(255, 255, 255, 200));
            p.setFont(QFont(QStringLiteral("Arial"), qMax(6, diskH - 10), QFont::Bold));
            p.drawText(dx, dy, dw, diskH - 2, Qt::AlignCenter,
                       QString::number(s.durchmesser()));
        }
    }

    // === 5. Animierende Scheibe auf Bézier-Bahn ===
    if (animating && !m_animPegs[animVon].istLeer()) {
        const int     von  = m_spiel.currentMoveVon();
        const int     nach = m_spiel.currentMoveNach();
        const Scheibe &s   = m_animPegs[von].oben();

        // Quell- und Zielposition der Scheibenmitte
        const int    srcJ  = m_animPegs[von].anzahl() - 1;
        const int    dstJ  = m_animPegs[nach].anzahl();
        const qreal  srcX  = qreal(pegX[von]);
        const qreal  dstX  = qreal(pegX[nach]);
        const qreal  srcCY = diskCenterY(srcJ);
        const qreal  dstCY = diskCenterY(dstJ);

        // Smoothstep als Geschwindigkeitsfunktion: f(t) = t²(3−2t)
        const qreal te = smoothstep(m_t);

        // X: geglättete lineare Interpolation
        const qreal ax = srcX + (dstX - srcX) * te;

        // Y: kubische Bézier mit Kontrollpunkten auf Freigabehöhe (Bogenbahn)
        const qreal ay = bezier1D(te, srcCY, clearanceCenterY, clearanceCenterY, dstCY);

        const int dw = diskWidth(s.durchmesser());
        const QRectF animRect(ax - dw / 2.0, ay - diskH / 2.0,
                              qreal(dw), qreal(diskH - 2));

        p.setBrush(s.farbe());
        p.setPen(QPen(s.farbe().darker(170), 1));
        p.drawRoundedRect(animRect, 5, 5);

        p.setPen(QColor(255, 255, 255, 200));
        p.setFont(QFont(QStringLiteral("Arial"), qMax(6, diskH - 10), QFont::Bold));
        p.drawText(animRect.toRect(), Qt::AlignCenter, QString::number(s.durchmesser()));
    }

    // === 6. Pflock-Beschriftungen ===
    p.setPen(QColor(170, 170, 195));
    p.setFont(QFont(QStringLiteral("Arial"), 9));
    const char *labels[] = {"Links", "Mitte", "Rechts"};
    for (int i = 0; i < 3; ++i) {
        p.drawText(pegX[i] - 28, baseY + 12, 56, 14,
                   Qt::AlignCenter, QString::fromUtf8(labels[i]));
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Berechnet den Animationsfortschritt t aus der vergangenen Zeit.
 *                Ist t ≥ 1.0: Zug abschließen, ggf. Beendet-Signal senden.
 *                Ruft update() für Neuzeichnen auf.
 */
void HanoiWidget::onAnimTimer()
{
    if (m_zustand != AnimationsZustand::Laeuft) {
        return;
    }

    const qint64 gesamtMs = m_zugElapsedBeforeMs
                            + (m_zugTimer.isValid() ? m_zugTimer.elapsed() : 0LL);
    const qreal zugDauerMs = qreal(m_einstellungen.zugdauerMs);

    m_t = qBound(0.0, gesamtMs / zugDauerMs, 1.0);

    if (m_t >= 1.0) {
        applyCurrentMove();
        m_zugElapsedBeforeMs = 0;
        m_zugTimer.restart();

        emit zugFortschrittGeaendert(m_spiel.zugzaehler(), m_spiel.gesamtzuege());

        if (!m_spiel.hatWeitereZuege()) {
            m_zustand = AnimationsZustand::Beendet;
            m_animTimer->stop();
            m_t = 0.0;
            update();
            emit zustandGeaendert(m_zustand);
            return;
        }

        m_t = 0.0;
    }

    update();
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Leert m_animPegs[1..2] und kopiert alle Scheiben aus dem
 *                Anfangszustand (m_spiel.pflock(0)) in m_animPegs[0].
 */
void HanoiWidget::initialisiereAnimationszustand()
{
    for (int i = 0; i < 3; ++i) {
        m_animPegs[i].leere();
    }
    for (const Scheibe &s : m_spiel.pflock(0).scheiben()) {
        m_animPegs[0].schiebeDrauf(s);
    }
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: –
 * Seiteneffekte: Liest von/nach aus dem Lazy-Generator, ruft executeAndAdvance()
 *                auf und überträgt den Zug auf m_animPegs.
 */
void HanoiWidget::applyCurrentMove()
{
    if (!m_spiel.hatWeitereZuege()) {
        return;
    }

    const int von  = m_spiel.currentMoveVon();
    const int nach = m_spiel.currentMoveNach();
    m_spiel.executeAndAdvance();

    if (!m_animPegs[von].istLeer()) {
        Scheibe s = m_animPegs[von].nehmeAb();
        m_animPegs[nach].schiebeDrauf(s);
    }
}
