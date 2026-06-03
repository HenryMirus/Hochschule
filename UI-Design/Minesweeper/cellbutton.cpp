#include "cellbutton.h"

#include <QMouseEvent>

/*
 * Autor: Henry Mirus
 * Eingabe: row    – Zeilenindex des Feldes im Spielgitter
 *          column – Spaltenindex des Feldes im Spielgitter
 *          parent – übergeordnetes Widget
 * Rückgabe: –
 * Seiteneffekte: Richtet Fokusrichtlinie und Mindestgröße ein.
 */
CellButton::CellButton(int row, int column, QWidget *parent)
    : QPushButton(parent)
    , m_row(row)
    , m_column(column)
{
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(24, 24);
    setText(QString());
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Zeilenindex dieses Feldes im Spielgitter
 * Seiteneffekte: –
 */
int CellButton::row() const
{
    return m_row;
}

/*
 * Autor: Henry Mirus
 * Eingabe: –
 * Rückgabe: Spaltenindex dieses Feldes im Spielgitter
 * Seiteneffekte: –
 */
int CellButton::column() const
{
    return m_column;
}

/*
 * Autor: Henry Mirus
 * Eingabe: event – Mausereignis mit Informationen über die gedrückte Taste
 * Rückgabe: –
 * Seiteneffekte: Sendet leftClicked, rightClicked oder middleClicked mit
 *                Zeilen- und Spaltenindex; andernfalls wird das Ereignis
 *                an die Basisklasse weitergegeben.
 */
void CellButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit leftClicked(m_row, m_column);
        return;
    }

    if (event->button() == Qt::RightButton) {
        emit rightClicked(m_row, m_column);
        return;
    }

    if (event->button() == Qt::MiddleButton) {
        emit middleClicked(m_row, m_column);
        return;
    }

    QPushButton::mousePressEvent(event);
}
