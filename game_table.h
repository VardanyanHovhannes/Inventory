#ifndef GAME_TABLE_H
#define GAME_TABLE_H

#include <QTableView>
#include <QMainWindow>
#include <QImage>
#include <QModelIndex>

class GameTable : public QTableView
{
    Q_OBJECT
public:
    GameTable(QMainWindow* parent = 0);
    void mousePressEvent(QMouseEvent* event);
    bool isTableIconPressed();
    void resetTableIconPressed();
    QModelIndex getSourceIndex();
private:
    bool m_isTableIconPressed;
    QModelIndex m_sourceIndex;
};

#endif // GAME_TABLE_H
