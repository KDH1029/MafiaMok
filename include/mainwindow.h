#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QGraphicsScene>
#include <QPointF>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QApplication>


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
    void addBubble(const QString &message, bool isSender);
    bool eventFilter(QObject *obj, QEvent *event);
    void on_actionOpen_Image_triggered();
    void drawBoard();
    void removeStone(int row, int col);
    void placeStone(int row, int col, int player);
    void showMafiaEffect(int row, int col, int stoneSize);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void get_udp();
    //void on_pushButton_clicked();
    void addBubble(const QString &message);
    void onGraphicsViewClicked(QPointF pos);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    QGraphicsScene* scene;
    QVector<QVector<int>> board; // 0:빈칸, 1:플레이어1, 2:플레이어2
    QVector<QVector<int>> mafia_bord; // 0:일반돌, 1:마피아
    int boardSize = 19;          // 오목판 19x19
    int cellSize = 60;    // 각 칸 크기
    QPointF mapToCell(const QPointF& pos);
    bool playchoice[3];
    int player;
    int player2;
    int player_life;
    int seduce_ticket;
    void send(QString str);
    void place(int x, int y);
    void win(void);
    void lose(void);
};



#endif // MAINWINDOW_H
