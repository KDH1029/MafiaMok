#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QImage>
#include <iostream>
#include <QGraphicsEllipseItem>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <QTimer>
#include <QDebug>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->viewport()->installEventFilter(this);

    // QString filename = "/home/yu/MafiaMook/my_image2.jpg";
    // QImage image(filename);

    playchoice = -1; // 처음엔 1번 플레이어 선택 상태

    player = 1;        // 이 컴퓨터는 1번 플레이어
    player2 = 2;       // 상대 플레이어
    player_life = 5;   // 목숨 5, 시민을 5번 없애면 패배
    seduce_ticket = 5; // 회유티켓. 일단 5로 하죠?

    board.resize(boardSize);
    for (int i = 0; i < boardSize; i++)
    {
        board[i].resize(boardSize);
        for (int j = 0; j < boardSize; j++)
        {
            board[i][j] = 0; // 빈칸 초기화
        }
    }

    this->field = new Field();
    this->udp = new Udp(this);
    connect(this->udp, &Udp::received, this, &MainWindow::handlePoint);

    drawBoard();

    udp->send("1,1,1");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handlePoint(Point p)
{
    qDebug()<<p.x;
    qDebug()<<p.y;
}

void MainWindow::drawBoard()
{
    scene->clear();
    for (int i = 0; i < boardSize; i++)
    {
        scene->addLine(0, i* cellSize, (boardSize - 1) * cellSize, i * cellSize); // 가로 //격자를 살짝 내리는 방향으로 .
        scene->addLine(i * cellSize, 0, i * cellSize, (boardSize - 1) * cellSize); // 세로
    }
}

QPointF MainWindow::mapToCell(const QPointF &pos)
{
    int col = int(pos.x()) / cellSize;
    int row = int(pos.y()) / cellSize;
    return QPointF(row, col); // ok
}

QGraphicsEllipseItem *stoneItems[20][20] = {nullptr}; // 최대 20x20 오목판 가정

void MainWindow::placeStone(int row, int col, int player)
{
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize || board[row][col] != 0)
    {
        qDebug() << "Wrong Access!";
        return;
    }

    board[row][col] = player;

    QBrush brush = (player == 1) ? Qt::black : Qt::white;
    int stoneSize = cellSize - 4;
    int centerX = col * cellSize;
    int centerY = row * cellSize;

    int x = centerX - stoneSize / 2;
    int y = centerY - stoneSize / 2;

    QGraphicsEllipseItem *stone = scene->addEllipse(x, y, stoneSize, stoneSize, QPen(), brush);
    stoneItems[row][col] = stone; // 저장
}

// 돌 제거 함수
void MainWindow::removeStone(int row, int col)
{
    if (stoneItems[row][col] != nullptr)
    {
        scene->removeItem(stoneItems[row][col]);
        delete stoneItems[row][col]; // 메모리 정리
        stoneItems[row][col] = nullptr;
        board[row][col] = 0;
    }
}

void MainWindow::showMafiaEffect(int row, int col, int stoneSize)
{

    // 이거는 시간 되면 넣어라. 간단한 효과인데 로그로 대체할 수 있다
    //  간단한 펄스 효과: QTimer로 크기 변화를 줌
    //  QTimer* timer = new QTimer(this);
    //  connect(timer, &QTimer::timeout, [effect]() {
    //      static bool growing = true;
    //      qreal scaleFactor = effect->scale();
    //      if (growing) {
    //          scaleFactor += 0.05;
    //          if (scaleFactor >= 1.5) growing = false;
    //      } else {
    //          scaleFactor -= 0.05;
    //          if (scaleFactor <= 1.0) growing = true;
    //      }
    //      effect->setScale(scaleFactor);
    //  });
    //  timer->start(50);  // 50ms마다
}

void MainWindow::onGraphicsViewClicked(QPointF pos)
{                                  // 여기서 발생한 모든 이벤트는 상대에게도 전송. 전송한 이벤트는 상대의 수신 함수에서 처리
    QPointF cell = mapToCell(pos); // clicked-->포인트 가져오기
    if (playchoice==0)
    {                                           // 돌 놓기 선택
        placeStone(cell.x(), cell.y(), player); // player마다 다른 돌
    }

    else if (playchoice==1)
    { // 돌 제거 선택
        if (board[cell.x()][cell.y()] != player)
        {
            ui->label->setText("Wrong Access"); // 상대방 돌 제거 시도->예외처리
        }
        else
        { // 자신 돌 제거-->무조건 제거
            removeStone(cell.x(), cell.y());
            ui->label->setText("Stone Distroied");
            if (mafia_bord[cell.x()][cell.y()] == player)
            {
                mafia_bord[cell.x()][cell.y()] = 0;
            } // 플레이어쪽 잠입 맞으면 마피아 삭제
            else
            {
                player_life--;
                cout<<"선량한 시민 돌이 사망했습니다..."<<endl;
                //addBubble("선량한 시민 돌이 사망했습니다...");
            }
        }
    }

    else if (playchoice==2)
    { // 돌 회유 선택
        if (board[cell.x()][cell.y()] != player || mafia_bord[cell.x()][cell.y()] != player)
        {
            ui->label->setText("What are you doing?");
        }
        else
        {
            ui->label->setText("Stone rehabilitated");
            mafia_bord[cell.x()][cell.y()] = player2;
            // 여기서 상대 PC에 전송: 회유 성공/실패는 표시하지 않음 시스템 상에서만 처리
        }
        seduce_ticket--; // 회유 쿠폰은 무조건 소비됨(횟수제한)
    }
    else
    {
        ui->label->setText("Choose One."); // 선택이 안됐거나 예외상황일시.
        return;
    }

}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{ // 마우스 이벤트 캡쳐 함수.
    if (obj == ui->graphicsView->viewport() && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPointF scenePos = ui->graphicsView->mapToScene(mouseEvent->pos());
        onGraphicsViewClicked(scenePos);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_radioButton_clicked()
{
    playchoice = 0; // 돌 놓기

}

void MainWindow::on_radioButton_2_clicked()
{
    playchoice = 1; // 돌 파괴
}

void MainWindow::on_radioButton_3_clicked()
{
    playchoice = 2; // 돌 회유
}
