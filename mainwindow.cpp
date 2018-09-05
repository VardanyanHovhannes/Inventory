#include "mainwindow.h"
#include "game_model.h"
#include "game_table.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QDir>
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDrag>
#include <QPainter>
#include <QGridLayout>
#include <QAbstractItemView>
#include <QWidget>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_imageIndex = 0;
    m_gameStarted = false;
    resize(800,650);
    this->setFixedSize(800,650);

    m_startButton = new QPushButton(this);
    m_startButton->setText("start");
    m_startButton->setGeometry(150,25,100,50);

    m_exitButton = new QPushButton(this);
    m_exitButton->setText("exit");
    m_exitButton->setGeometry(300,25,100,50);

    m_menuButton = new QPushButton(this);
    m_menuButton->setText("menu");
    m_menuButton->setGeometry(600,500,100,50);
    m_menuButton->setEnabled(false);

    m_saveButton = new QPushButton(this);
    m_saveButton->setText("save");
    m_saveButton->setGeometry(600,25,100,50);
    m_saveButton->setEnabled(false);

    m_loadButton = new QPushButton(this);
    m_loadButton->setText("load");
    m_loadButton->setGeometry(600,100,100,50);

    m_table = new GameTable(this);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setModel( m_model = new GameModel );
    m_table->setGeometry(50,100,450,450);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setVisible(false);
    m_table->setEnabled(false);

    m_label = new QLabel(this);
    m_label->setGeometry(600,300,150,150);
    m_label->setEnabled(false);

    QString path ="/home/myusername/Folder1";
    QDir direct(path);
    QStringList nameFilter;
    nameFilter << "*.png" << "*.jpg" << "*.JPEG";
    QFileInfoList list = direct.entryInfoList(nameFilter, QDir::Files);

    for(int i=0; i < list.size(); ++i)
    {
        m_images.push_back(QImage(list[i].filePath()).scaled(m_label->width(), m_label->height()));
    }
    m_label->setPixmap(QPixmap::fromImage(m_images[m_imageIndex]));
    m_model->setImages(m_images);

    connect(m_startButton, SIGNAL(clicked()), this, SLOT(startGame()));
    connect(m_exitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_menuButton, SIGNAL(clicked()), this, SLOT(openMenu()));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(saveGame()));
    connect(m_loadButton, SIGNAL(clicked()), this, SLOT(loadGame()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QModelIndex index = m_table->indexAt(event->pos() - (m_table->geometry().topLeft()));

    if(index.isValid())
    {
        if(!(m_table->isTableIconPressed()))
        {
            m_model->setImage(index, m_imageIndex);
            m_imageIndex = qrand()%3;
            m_label->setPixmap(QPixmap::fromImage(m_images[m_imageIndex]));
        }
        else
        {
            m_model->moveImage((m_table->getSourceIndex()), index);
        }
        m_table->resetTableIconPressed();
    }
    event->acceptProposedAction();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QLabel* child = dynamic_cast<QLabel*>(childAt(event->pos()));

    if (!child || !(child->pixmap()) || !m_gameStarted)
        return;
    QPixmap pixmap = *child->pixmap();
    QMimeData* mimeData = new QMimeData;
    mimeData->setImageData(pixmap);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());
    drag->exec(Qt::MoveAction);
}

void MainWindow::startGame()
{
    m_imageIndex = 0;
    m_gameStarted = true;
    m_model->resetModel();

    m_startButton->hide();
    m_exitButton->hide();
    m_table->setEnabled(true);
    m_label->setEnabled(true);
    m_menuButton->setEnabled(true);
    m_saveButton->setEnabled(true);

    m_label->setPixmap(QPixmap::fromImage(m_images[m_imageIndex]));
    setAcceptDrops(true);
}

void MainWindow::openMenu()
{
    m_gameStarted = false;

    m_startButton->show();
    m_exitButton->show();
    m_table->setEnabled(false);
    m_label->setEnabled(false);
    m_menuButton->setEnabled(false);
    m_saveButton->setEnabled(false);
}

void MainWindow::addImage(QSqlQuery &q, int row, int column, int imageIndex)
{
    q.addBindValue(row);
    q.addBindValue(column);
    q.addBindValue(imageIndex);
    q.exec();
}

void MainWindow::loadGame()
{
    QSettings settings( "settings_demo.conf", QSettings::IniFormat );
    settings.beginGroup( "VectorIndex" );
    m_imageIndex = settings.value( "image_index", -1 ).toInt();
    settings.endGroup();
    m_gameStarted = true;
    m_model->resetModel();

    m_startButton->hide();
    m_exitButton->hide();
    m_table->setEnabled(true);
    m_label->setEnabled(true);
    m_menuButton->setEnabled(true);
    m_saveButton->setEnabled(true);
    m_label->setPixmap(QPixmap::fromImage(m_images[m_imageIndex]));
    setAcceptDrops(true);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("/home/myusername/Folder1/imagesDB");

    if(!m_db.open())
        qDebug() << m_db.lastError();

    QModelIndex index;

    QSqlQuery q(m_db);
    QMap<QModelIndex, QVector<int>> tableContent;

    q.exec("SELECT row, column, imageIndex FROM imagesTable");
    while (q.next())
    {
        index = m_model->index(q.value(0).toInt(), q.value(1).toInt(), QModelIndex());

        if(tableContent.find(index) == tableContent.end())
        {
            QVector<int> new_image_vector;
            new_image_vector.push_back(q.value(2).toInt());
            tableContent[index] = new_image_vector;
        }
        else
        {
            tableContent[index].push_back(q.value(2).toInt());
        }
    }
    QString connection;
    connection = m_db.connectionName();
    m_db.close();
    m_db = QSqlDatabase();
    m_db.removeDatabase(connection);

    m_model->setTableContent(tableContent);
}
QSqlError MainWindow::initDb()
{
    if (!QSqlDatabase::drivers().contains("QSQLITE"))
       qDebug() << "Unable to load databaseThis demo needs the SQLITE driver";

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("/home/myusername/Folder1/imagesDB");

    if (!m_db.open())
        return m_db.lastError();

    QStringList tables = m_db.tables();
    QSqlQuery q(m_db);
    if (tables.contains("imagesTable", Qt::CaseInsensitive))
    {
        if (!q.exec(QLatin1String("DROP TABLE imagesTable")))
           qDebug() << q.lastError();
    }
    if (!q.exec(QLatin1String("create table imagesTable(id integer primary key, row integer, column integer, imageIndex integer)")))
        qDebug() << q.lastError();

    q.exec("DELETE from imagesTable");

    QMap<QModelIndex, QVector<int> > map(m_model->getTableContent());
    QMap<QModelIndex, QVector<int>>::const_iterator i;

    for(i = map.begin(); i != map.end(); ++i)
    {
        QVector<int>::const_iterator j;
        for( j = i.value().begin(); j != i.value().end(); ++j)
        {
            if (!q.prepare(QLatin1String("insert into imagesTable(row, column, imageIndex) values(?, ?, ?)")))
            {
                qDebug() << q.lastError();
                return q.lastError();
            }
            addImage(q, i.key().row(), i.key().column(), *j );
        }
    }
    QString connection;
    connection = m_db.connectionName();
    m_db.close();
    m_db = QSqlDatabase();
    m_db.removeDatabase(connection);

    return QSqlError();
}

void MainWindow::saveGame()
{
    QSettings settings( "settings_demo.conf", QSettings::IniFormat );

    settings.beginGroup( "VectorIndex" );
    settings.setValue( "image_index", m_imageIndex );
    settings.endGroup();

    QSqlError err;
    err = initDb();
}
