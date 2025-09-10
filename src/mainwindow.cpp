#include "setting.h"

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

    playchoice = 0;    // 처음엔 1번 플레이어 선택 상태
    player_life = 5;   // 목숨 5, 시민을 5번 없애면 패배
    seduce_ticket = 5; // 회유티켓. 일단 5로 하죠?
    restart = false;

    this->field = new Field();
    this->udp = new Udp(this);
    connect(this->udp, &Udp::pointReceived, this, &MainWindow::handlePoint);
    connect(this->udp, &Udp::cmdReceived, this, &MainWindow::handleCmd);

    this->stoneItems[20][20] = {nullptr}; // 최대 20x20 오목판 가정
    drawBoard();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handlePoint(Point p)
{
    qDebug() << p.x;
    qDebug() << p.y;

    if (!this->field->turn)
    {
        switch (p.value)
        {
        case 0:
            if (this->field->remove(p.x, p.y))
            {
                removeStone(p.x, p.y);
                this->field->turn = true;
            }
            break;
        case 1:
        case 2:
            placeStone(p.x, p.y, p.value);
            break;
        }
    case -1:
        if (this->field->seduce(p.x, p.y))
        {
            this->field->turn = true;
        }
        break;
    }
    else
    {
        // err: 턴 오류
    }
}

void MainWindow::handleCmd(const QString &cmd)
{
    if (cmd == "WIN")
    {
        qDebug() << "You Lose!";
        this->field->turn = false;
    }
    else if (cmd == "LOSE")
    {
        qDebug() << "You Win!";
        this->field->turn = false;
    }
}

void MainWindow::drawBoard()
{
    scene->clear();
    for (int i = 0; i < boardSize; i++)
    {
        scene->addLine(0, i * cellSize, (boardSize - 1) * cellSize, i * cellSize); // 가로 //격자를 살짝 내리는 방향으로 .
        scene->addLine(i * cellSize, 0, i * cellSize, (boardSize - 1) * cellSize); // 세로
    }
    scene->addLine(-1, -1 * cellSize, (boardSize - 1) * cellSize, -1 * cellSize);
    scene->addLine(19, 19 * cellSize, (boardSize - 1) * cellSize, 19 * cellSize); // 가로 //격자를 살짝 내리는 방향으로 .
    scene->addLine(19 * cellSize, 0, 19 * cellSize, (boardSize - 1) * cellSize);
    scene->addLine(-1 * cellSize, 0, -1 * cellSize, (boardSize - 1) * cellSize);
}

QPointF MainWindow::mapToCell(const QPointF &pos)
{
    int col = (int)(double(pos.x()) / cellSize + 0.5);
    int row = (int)(double(pos.y()) / cellSize + 0.5);
    return QPointF(row, col); // ok
}

void MainWindow::placeStone(int row, int col, int value)
{
    if (this->field->place(row, col, value) == false)
    {
        qDebug() << "Wrong Access!";
        return;
    }

    QBrush brush = (value % 2) ? Qt::black : Qt::white;
    int stoneSize = cellSize - 4;
    int centerX = col * cellSize;
    int centerY = row * cellSize;

    int x = centerX - stoneSize / 2;
    int y = centerY - stoneSize / 2;

    QGraphicsEllipseItem *stone = scene->addEllipse(x, y, stoneSize, stoneSize, QPen(), brush);
    stoneItems[row][col] = stone; // 저장

    if (this->field->check())
    {
        this->field->turn = false;
        if (value = this->field->team)
            qDebug() << "Win!"; // 플레이어2 승리조건
        else
            qDebug() << "Lose!"; // 플레이어1 승리조건
    }
}

// 돌 제거 함수
void MainWindow::removeStone(int row, int col)
{
    if (stoneItems[row][col] != nullptr)
    {
        scene->removeItem(stoneItems[row][col]);
        delete stoneItems[row][col]; // 메모리 정리
        stoneItems[row][col] = nullptr;
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
    if (cell.x() > boardSize || cell.x() < 0 || cell.y() > boardSize || cell.y() < 0)
    {
        ui->label->setText("You're Out!");
        return;
    } // 보드판 외 다른 곳 클릭, 예외처리
    int state = this->field->board[cell.y()][cell.x()]->value;
    if (playchoice == 0 && this->field->turn)
    { // 돌 놓기 선택
        ui->label->setText("You placed stone");
        placeStone(cell.x(), cell.y(), this->field->team); // player마다 다른 돌
        this->udp->send(QString("%1,%2,%3").arg(cell.x()).arg(cell.y()).arg(this->field->team));
    }

    else if (playchoice == 1 && this->field->turn)
    { // 돌 제거 선택
#if Player
        if (state != 2 && state != 3)
#else
        if (state != 1 && state != 4)
#endif
        {
            ui->label->setText("Can't reduce it"); // 자신 돌이 아닌 것 제거 시도->예외처리
        }
        else
        { // 자신 돌 제거-->무조건 제거
            if (this->field->remove(cell.x(), cell.y()))
            {
                this->udp->send(QString("%1,%2,0").arg(cell.x()).arg(cell.y()));
                removeStone(cell.x(), cell.y());
                ui->label->setText("Stone Distroied");
                if (this->field->team == state)
                {
                    player_life--;
                    cout << "선량한 시민 돌이 사망했습니다..." << endl;
                    if (player_life <= 0)
                    {
                        qDebug() << "You Lose!"; // 사용자 패배 조건(목숨이 깎이는 경우는 돌을 잘못 지우는 경우밖에 없으므로)
                        this->udp->send("LOSE");
                        this->field->turn = false;
                    }
                }
            }
        }
    }

    else if (playchoice == 2 && this->field->turn && seduce_ticket >= 1) // 회유 쿠폰 있을 시에만 가능
    {                                                                    // 돌 회유 선택
        if (state == 3 - this->field->team)
        {
            if (this->field->seduce(cell.x(), cell.y()))
            {
                this->udp->send(QString("%1,%2,-1").arg(cell.x()).arg(cell.y()));
                ui->label->setText("Stone Rehabilitated");
            }
            seduce_ticket--; // 회유 쿠폰은 무조건 소비됨(횟수제한)
        }
        else
        {
            ui->label->setText("What Are You Doing?");
        }
    }
    else
    {
        ui->label->setText("Wrong Access"); // 선택이 안됐거나 예외상황일시.
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

void MainWindow::End_event(bool identify)
{
    if (identify)
    {
        this->field->turn = false;
    }
    else
        this->field->turn = true; // 다시 시작.
}

void MainWindow::on_pushButton_clicked()
{
    QApplication::quit();
}

void MainWindow::on_pushButton_2_clicked()
{
    if (restart)
    {
        ;
    }
}
