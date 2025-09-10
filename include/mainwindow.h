#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPointF>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QApplication>

#include "field.h"
#include "udp.h"

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
    QGraphicsEllipseItem *stoneItems[20][20];
    bool eventFilter(QObject *obj, QEvent *event);
    void on_actionOpen_Image_triggered();
    void drawBoard();
    void removeStone(int row, int col);
    void placeStone(int row, int col, int value);
    void showMafiaEffect(int row, int col, int stoneSize);
    void addBubble(const QString &message, bool isSender);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handlePoint(Point p);
    void handleCmd(const QString &cmd);
    
    void onGraphicsViewClicked(QPointF pos);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    Field *field;
    Udp *udp;

    QGraphicsScene* scene;
    int boardSize = 19;          // 오목판 19x19
    int cellSize = 50;    // 각 칸 크기
    QPointF mapToCell(const QPointF& pos);

    int playchoice;
    int player_life;
    int seduce_ticket;
};



#endif // MAINWINDOW_H
