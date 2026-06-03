#ifndef CELLBUTTON_H
#define CELLBUTTON_H

#include <QPushButton>

class QMouseEvent;

/*
 * Autor: Henry Mirus
 * Zweck: Schaltfläche für ein einzelnes Spielfeld im Minesweeper-Gitter.
 *        Erbt von QPushButton und unterscheidet zwischen linkem, rechtem und
 *        mittlerem Mausklick durch eigene Signale.
 */
class CellButton : public QPushButton
{
    Q_OBJECT

public:
    explicit CellButton(int row, int column, QWidget *parent = nullptr);

    int row() const;
    int column() const;

signals:
    void leftClicked(int row, int column);
    void rightClicked(int row, int column);
    void middleClicked(int row, int column);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_row;
    int m_column;
};

#endif // CELLBUTTON_H
