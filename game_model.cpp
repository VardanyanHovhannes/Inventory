#include "game_model.h"
#include <QTableView>

#include <QAbstractTableModel>
#include <QFileInfoList>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>
#include <QPainter>
#include <QLabel>

GameModel::GameModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

int GameModel::rowCount(const QModelIndex &parent) const
{
    return 3;
}

int GameModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant GameModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::CheckStateRole && m_tableContent.find(index) != m_tableContent.end())
    {
        QString str = QString::number(m_tableContent[index].size());
        QImage image = m_images[m_tableContent[index].last()];
        QPainter* painter =new QPainter(&image);
        painter->setPen(Qt::red);
        painter->setFont(QFont("Arial", 30));
        painter->drawText(10, 35, str);
        delete painter;

        return image;
    }
    else
        return QVariant();
}

bool GameModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return true;
}

void GameModel::setImage(QModelIndex& index, int image_index)
{
    if(m_tableContent.find(index) == m_tableContent.end())
    {
        QVector<int> new_image_vector;
        new_image_vector.push_back(image_index);
        m_tableContent[index] = new_image_vector;
        emit dataChanged(index, index);
    }
    else
    {
        m_tableContent[index].push_back(image_index);
        emit dataChanged(index, index);
    }
}

void GameModel::moveImage(QModelIndex sourceIndex, QModelIndex destinationIndex)
{
    if(m_tableContent.find(sourceIndex) != m_tableContent.end())
    {
        int lastInVector = m_tableContent[sourceIndex].last();

        if(m_tableContent[sourceIndex].size() == 1)
            m_tableContent.remove(sourceIndex);
        else
            m_tableContent[sourceIndex].removeLast();

        if(m_tableContent.find(destinationIndex) == m_tableContent.end())
        {
            QVector<int> new_image_vector;
            new_image_vector.push_back(lastInVector);
            m_tableContent[destinationIndex] = new_image_vector;
            emit dataChanged(sourceIndex, sourceIndex);
            emit dataChanged(destinationIndex, destinationIndex);
        }
        else
        {
            m_tableContent[destinationIndex].push_back(lastInVector);
            emit dataChanged(sourceIndex, sourceIndex);
            emit dataChanged(destinationIndex, destinationIndex);
        }
    }
}

void GameModel::resetModel()
{
    beginResetModel();
    m_tableContent.clear();
    endResetModel();
}

QMap<QModelIndex,QVector<int>> GameModel::getTableContent()
{
    return m_tableContent;
}

void GameModel::setImages(QVector<QImage> images)
{
    m_images = images;
}

void GameModel::setTableContent(QMap<QModelIndex, QVector<int> > tableContent)
{
    m_tableContent = tableContent;
}
