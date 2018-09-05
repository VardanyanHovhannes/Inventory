#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game_model.h"
#include "game_table.h"

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QDrag>
#include <QMutex>
#include <QTableView>
#include <QtSql>

class QDragEnterEvent;
class QDropEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void addImage(QSqlQuery &q, int row, int column, int imageIndex);
    QSqlError initDb();
public slots:
    void startGame();
    void openMenu();
    void saveGame();
    void loadGame();
private:
    GameTable* m_table;
    GameModel* m_model;
    QPushButton* m_startButton;
    QPushButton* m_exitButton;
    QPushButton* m_menuButton;
    QPushButton* m_saveButton;
    QPushButton* m_loadButton;
    QLabel* m_label;
    QVector<QImage> m_images;
    int m_imageIndex;
    bool m_gameStarted;
    QSqlDatabase m_db;
};

#endif // MAINWINDOW_H



