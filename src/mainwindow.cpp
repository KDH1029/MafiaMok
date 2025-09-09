#define IP "192.168.0.37"

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QUdpSocket>
<<<<<<< HEAD
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
using namespace std;
=======

#include <iostream>

>>>>>>> 40943c50967b16afd0c10e393916371be9ee2eb3
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->viewport()->installEventFilter(this);

    board.resize(boardSize);
    for(int i=0; i<boardSize; i++){
        board[i].resize(boardSize);
        for(int j=0; j<boardSize; j++){
            board[i][j] = 0; // 빈칸 초기화
        }
    }

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 9999);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::get_udp);

<<<<<<< HEAD
    drawBoard();

    //QWidget *container = new QWidget(this);
    //container->setMinimumSize(1, 1);
    //container->setFixedWidth(ui->scrollArea->width());
    //ui->scrollArea->setWidget(container);
    //ui->scrollArea->setWidgetResizable(false);
    //ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
=======
    this->field = new Field();
>>>>>>> 40943c50967b16afd0c10e393916371be9ee2eb3
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::get_udp()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString message = QString::fromUtf8(datagram);
<<<<<<< HEAD
        //addBubble(message, false);
    }
}

//void MainWindow::on_pushButton_clicked()
//{
    //QString message = ui->textEdit->toPlainText();
    //if (message.isEmpty())
        //return;

    //udpSocket->writeDatagram(message.toUtf8(), QHostAddress("192.168.0.37"), 9999);
    //addBubble(message, true);
    //ui->textEdit->clear();
//}

//void MainWindow::addBubble(const QString &message, bool isSender)
//{
    //QLabel *label = new QLabel(message);
    //label->setWordWrap(true);
    //label->setMaximumWidth(ui->scrollArea->width() - 60);
    //label->adjustSize();
    //label->setStyleSheet(isSender ? "background-color: #FFFFFF; border-radius: 10px; padding: 8px;" : "background-color: #FEE500; border-radius: 10px; padding: 8px;");
    //label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //QHBoxLayout *layout = new QHBoxLayout;
    //layout->setContentsMargins(10, 5, 10, 5);

    //if (isSender)
    //{
        //layout->addStretch();
        //layout->addWidget(label);
    //}
    //else
    //{
        //layout->addWidget(label);
        //layout->addStretch();
    //}

   // QWidget *bubbleWidget = new QWidget;
    //bubbleWidget->setLayout(layout);

    //QWidget *container = ui->scrollArea->widget();
    //bubbleWidget->setParent(container);
    //bubbleWidget->show();

    //int yOffset = 0;
    //for (QObject *child : container->children())
    //{
        //QWidget *w = qobject_cast<QWidget *>(child);
        //if (w && w != bubbleWidget)
        //{
            //yOffset += w->height() + 10;
        //}
    //}

    //int containerWidth = container->width();
    //int bubbleWidth = bubbleWidget->sizeHint().width();
    //int xOffset = isSender ? containerWidth - bubbleWidth - 10 : 10;

    //bubbleWidget->move(xOffset, yOffset);
    //container->resize(container->width(), yOffset + bubbleWidget->height() + 20);
    //ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
//}
void MainWindow::drawBoard() {
    scene->clear();
    for(int i=0;i<boardSize;i++){
        scene->addLine(0, i*cellSize, (boardSize-1)*cellSize, i*cellSize); // 가로
        scene->addLine(i*cellSize, 0, i*cellSize, (boardSize-1)*cellSize); // 세로
    }
}

QPointF MainWindow::mapToCell(const QPointF& pos) {
    int col = int(pos.x()) / cellSize;
    int row = int(pos.y()) / cellSize;
    return QPointF(row, col);  //ok
}

void MainWindow::placeStone(int row, int col, int player) {
    if(row < 0 || row > boardSize || col < 0 || col > boardSize || board[row][col] != 0) cout<<"Wrong Access!"<<endl;
    else {board[row][col] = player;

    QBrush brush = (player == 1) ? Qt::black : Qt::white;
    int stoneSize = cellSize - 4; // 돌 크기 (살짝 여백)
    int centerX = col * cellSize;
    int centerY = row * cellSize;

    int x = centerX - stoneSize / 2;
    int y = centerY - stoneSize / 2;

    scene->addEllipse(x, y, stoneSize, stoneSize, QPen(), brush);}
}

void MainWindow::onGraphicsViewClicked(QPointF pos) {
    QPointF cell = mapToCell(pos); //clicked-->포인트 가져오기
    placeStone(cell.x(), cell.y(), 1); // 1번 플레이어 예시
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->graphicsView->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = ui->graphicsView->mapToScene(mouseEvent->pos());
        onGraphicsViewClicked(scenePos);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}
=======
        QStringList arr = message.split(",");

        if (arr.size() == 3) {
            this->field->turn = true;
            int x = arr[0].toInt();
            int y = arr[1].toInt();
            int value = arr[2].toInt();
            this->field->place(x, y, value);
            if (this->field->check()) {
                this->lose();
            }
        }
    }
}

void MainWindow::send(QString str)
{
    udpSocket->writeDatagram(str.toUtf8(), QHostAddress(IP), 9999);
}

void MainWindow::place(int x, int y)
{
    //자신 턴인지 확인
    if (this->field->turn)
    {
        //빈 공간인지 확인
        if(this->field->place(x,y,this->field->color)){
            send(QString("%1,%2").arg(x).arg(y));
            this->field->turn = false;
            //승리 여부 판단
            if(this->field->check()){
                this->win();
            }
        }
    }
}

void MainWindow::win() {
    std::cout<<"win";
}

void MainWindow::lose() {
    std::cout<<"lose";
}
>>>>>>> 40943c50967b16afd0c10e393916371be9ee2eb3
