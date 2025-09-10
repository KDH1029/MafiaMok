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

#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>

#include <random>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->viewport()->installEventFilter(this);

    playchoice = 0;    // 처음엔 1번 플레이어 선택 상태
    player_life = 5;   // 목숨 5, 시민을 5번 없애면 패배
    seduce_ticket = 5; // 회유티켓 5

    this->field = new Field();
    this->udp = new Udp(this);
    connect(this->udp, &Udp::pointReceived, this, &MainWindow::handlePoint);
    connect(this->udp, &Udp::cmdReceived, this, &MainWindow::handleCmd);

    QWidget *container = new QWidget(this);
    container->setMinimumSize(1, 1);
    container->setFixedWidth(ui->scrollArea->width());
    ui->scrollArea->setWidget(container);
    ui->scrollArea->setWidgetResizable(false);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->stoneItems[20][20] = {nullptr}; // 최대 20x20 오목판 가정
    drawBoard();

#if Player
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    this->field->turn = dist(gen);
    if (this->field->turn)
    {
        this->field->team = 1;
        addBubble("You are the First Player", true);
        this->udp->send("SECOND");
    }
    else
    {
        this->field->team = 2;
        addBubble("You are the Second Player", true);
        this->udp->send("FIRST");
    }
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handlePoint(Point p)
{
    if (!this->field->turn)
    {
        switch (p.value)
        {
        case 0:
            if (this->field->remove(p.x, p.y))
            {
                removeStone(p.x, p.y);
                this->field->turn = true;
                addBubble(QString("Removed stone at (%1, %2)").arg(p.x).arg(p.y), false);
            }
            break;
        case 1:
        case 2:
            placeStone(p.x, p.y, p.value);
            break;
        case -1:
            if (this->field->seduce(p.x, p.y))
            {
                this->field->turn = true;
                addBubble(QString("Seduced"), false);
                if (this->field->check())
                {
                    this->field->turn = false;
                    addBubble("You Lose!", false);
                }
            }
            break;
        }
    }
}

void MainWindow::handleCmd(const QString &cmd)
{
    if (cmd == "WIN")
    {
        this->field->turn = false;
        addBubble("You Lose!", false);
    }
    else if (cmd == "LOSE")
    {
        this->field->turn = false;
        addBubble("You Win!", false);
    }
    else if (cmd == "RESET")
    {
        delete this->field;
        this->field = new Field();

        player_life = 5;
        seduce_ticket = 5;

        scene->clear();
        drawBoard();

        for (int r = 0; r < 20; ++r)
            for (int c = 0; c < 20; ++c)
                stoneItems[r][c] = nullptr;

        ui->label->setText("Game restarted!");

        ui->radioButton->setAutoExclusive(false);
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setAutoExclusive(false);
        ui->radioButton_2->setChecked(false);
        ui->radioButton_3->setAutoExclusive(false);
        ui->radioButton_3->setChecked(false);

#if Player
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 1);

        this->field->turn = dist(gen);
        if (this->field->turn)
        {
            this->field->team = 1;
            addBubble("You are the First Player", true);
            this->udp->send("FIRST");
        }
        else
        {
            this->field->team = 2;
            addBubble("You are the Second Player", true);
            this->udp->send("SECOND");
        }
#endif
    }
    else if (cmd == "QUIT")
    {
        QApplication::quit();
    }
    else if (cmd == "FIRST")
    {
        this->field->turn = false;
        this->field->team = 2;
        addBubble("You are the Second Player", false);
    }
    else if (cmd == "SECOND")
    {
        this->field->turn = true;
        this->field->team = 1;
        addBubble("You are the First Player", false);
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
    scene->addLine(19, 19 * cellSize, (boardSize - 1) * cellSize, 19 * cellSize); // 위젯 흔들림을 막기 위한 선들
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
        return;
    }
    addBubble(QString("Placed stone at (%1, %2)").arg(row).arg(col), value == this->field->team);
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
        if (value == this->field->team || value == this->field->team + 2)
        {
            addBubble("You Win!", true);
        }
        else
        {
            addBubble("You Lose!", true);
        }
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

        if ((this->field->team == 2 && state != 2 && state != 3) || (this->field->team == 1 && state != 1 && state != 4))
        {
            ui->label->setText("Can't reduce it"); // 자신 돌이 아닌 것 제거 시도->예외처리
        }
        else
        { // 자신 돌 제거-->무조건 제거
            if (this->field->remove(cell.x(), cell.y()))
            {
                this->udp->send(QString("%1,%2,0").arg(cell.x()).arg(cell.y()));
                addBubble(QString("Removed stone at (%1, %2)").arg(cell.x()).arg(cell.y()), true);
                removeStone(cell.x(), cell.y());
                ui->label->setText("Stone Distroied");
                if (this->field->team == state)
                {
                    player_life--;
                    addBubble("선량한 시민 돌이 사망했습니다...", true);
                    if (player_life <= 0)
                    {
                        this->udp->send("LOSE");
                        addBubble("You Lose!", true);
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
                addBubble(QString("Seduce stone at (%1, %2)").arg(cell.x()).arg(cell.y()), true);
                ui->label->setText("Stone Rehabilitated");
                if (this->field->check())
                {
                    for (int i = 0; i < 8; i++)
                    {
                    }
                    this->field->turn = false;
                    addBubble("You Win!", true);
                }
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
    // int randomValue = qsrand()%3;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{ // 마우스 이벤트 캡쳐 함수
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
    ui->radioButton_2->setAutoExclusive(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_3->setAutoExclusive(false);
    ui->radioButton_3->setChecked(false);
}

void MainWindow::on_radioButton_2_clicked()
{
    playchoice = 1; // 돌 파괴
    ui->radioButton->setAutoExclusive(false);
    ui->radioButton->setChecked(false);
    ui->radioButton_3->setAutoExclusive(false);
    ui->radioButton_3->setChecked(false);
}

void MainWindow::on_radioButton_3_clicked()
{
    playchoice = 2; // 돌 회유
    ui->radioButton->setAutoExclusive(false);
    ui->radioButton->setChecked(false);
    ui->radioButton_2->setAutoExclusive(false);
    ui->radioButton_2->setChecked(false);
}
void MainWindow::on_pushButton_clicked()
{
    QApplication::quit();
    this->udp->send("QUIT");
}

void MainWindow::on_pushButton_2_clicked()
{
    delete this->field;
    this->field = new Field();

    player_life = 5;
    seduce_ticket = 5;

    scene->clear();
    drawBoard();

    for (int r = 0; r < 20; ++r)
        for (int c = 0; c < 20; ++c)
            stoneItems[r][c] = nullptr;

    ui->label->setText("Game restarted!");
    this->udp->send("RESET");

    ui->radioButton->setAutoExclusive(false);
    ui->radioButton->setChecked(false);
    ui->radioButton_2->setAutoExclusive(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_3->setAutoExclusive(false);
    ui->radioButton_3->setChecked(false);

#if Player
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    this->field->turn = dist(gen);
    if (this->field->turn)
    {
        this->field->team = 1;
        addBubble("You are the First Player", true);
        this->udp->send("FIRST");
    }
    else
    {
        this->field->team = 2;
        addBubble("You are the Second Player", true);
        this->udp->send("SECOND");
    }
#endif
}

void MainWindow::addBubble(const QString &message, bool isSender)
{
    QLabel *label = new QLabel(message);
    label->setWordWrap(true);
    label->setMaximumWidth(ui->scrollArea->width() - 60);
    label->adjustSize();
    label->setStyleSheet(isSender ? "background-color: #FFFFFF; border-radius: 10px; padding: 8px;" : "background-color: #FEE500; border-radius: 10px; padding: 8px;");
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(10, 5, 10, 5);

    if (isSender)
    {
        layout->addStretch();
        layout->addWidget(label);
    }
    else
    {
        layout->addWidget(label);
        layout->addStretch();
    }

    QWidget *bubbleWidget = new QWidget;
    bubbleWidget->setLayout(layout);

    QWidget *container = ui->scrollArea->widget();
    bubbleWidget->setParent(container);
    bubbleWidget->show();

    int yOffset = 0;
    for (QObject *child : container->children())
    {
        QWidget *w = qobject_cast<QWidget *>(child);
        if (w && w != bubbleWidget)
        {
            yOffset += w->height() + 10;
        }
    }

    int containerWidth = container->width();
    int bubbleWidth = bubbleWidget->sizeHint().width();
    int xOffset = isSender ? containerWidth - bubbleWidth - 10 : 10;

    bubbleWidget->move(xOffset, yOffset);
    container->resize(container->width(), yOffset + bubbleWidget->height() + 20);
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}
