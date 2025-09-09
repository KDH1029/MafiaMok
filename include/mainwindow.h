#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
<<<<<<< HEAD
#include <QGraphicsScene>
#include <QPointF>
#include <QMouseEvent>
#include <iostream>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
=======
#include "field.h"
>>>>>>> 40943c50967b16afd0c10e393916371be9ee2eb3

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
<<<<<<< HEAD
    void addBubble(const QString &message, bool isSender);
    bool eventFilter(QObject *obj, QEvent *event);
    void on_actionOpen_Image_triggered();
=======
    Field *field;
>>>>>>> 40943c50967b16afd0c10e393916371be9ee2eb3
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void get_udp();
<<<<<<< HEAD
    //void on_pushButton_clicked();
    //void addBubble(const QString &message, bool isSender);
    void onGraphicsViewClicked(QPointF pos);
=======
>>>>>>> 40943c50967b16afd0c10e393916371be9ee2eb3

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
<<<<<<< HEAD
    QGraphicsScene* scene;
    QVector<QVector<int>> board; // 0:빈칸, 1:플레이어1, 2:플레이어2
    int boardSize = 19;          // 오목판 19x19
    int cellSize = 60;           // 각 칸 크기
    void drawBoard();
    void placeStone(int row, int col, int player);
    QPointF mapToCell(const QPointF& pos);
=======
    void send(QString str);
    void place(int x, int y);
    void win(void);
    void lose(void);
>>>>>>> 40943c50967b16afd0c10e393916371be9ee2eb3
};

#endif // MAINWINDOW_H
