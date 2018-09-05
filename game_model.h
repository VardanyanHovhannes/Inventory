#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include <QAbstractTableModel>
#include <QImage>
#include <QVector>
#include <QList>
#include <QMap>
#include <QPainter>

class GameModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    GameModel( QObject* parent = 0 );
    int rowCount( const QModelIndex& parent ) const;
    int columnCount( const QModelIndex& parent ) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void setImage(QModelIndex& index, int image_index);
    void moveImage(QModelIndex sourceIndex, QModelIndex destinationIndex);
    void resetModel();
    void setImages(QVector<QImage> images);
    QMap<QModelIndex,QVector<int>> getTableContent();
    void setTableContent(QMap<QModelIndex, QVector<int>> tableContent);
private:
    QVector<QImage> m_images;
    QMap<QModelIndex, QVector<int>> m_tableContent;
};


#endif // GAME_MODEL_H


