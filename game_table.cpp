#include "game_table.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QLabel>
#include <QMimeData>
#include <QDrag>

GameTable::GameTable(QMainWindow *parent)
    : QTableView(parent)
{
    m_isTableIconPressed = false;
}

void GameTable::mousePressEvent(QMouseEvent *event)
{
    m_isTableIconPressed = true;

    QModelIndex index = this->indexAt(event->pos());
    m_sourceIndex = index;
    QImage image = qvariant_cast<QImage>(index.data());

    QPixmap pixmap = QPixmap::fromImage(image);
    QMimeData* mimeData = new QMimeData;
    mimeData->setImageData(pixmap);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - visualRect(index).topLeft());

    drag->exec(Qt::MoveAction);
}

bool GameTable::isTableIconPressed()
{
    return m_isTableIconPressed;
}

void GameTable::resetTableIconPressed()
{
    m_isTableIconPressed = false;
}

QModelIndex GameTable::getSourceIndex()
{
    return m_sourceIndex;
}
