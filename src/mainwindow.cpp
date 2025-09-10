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

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->viewport()->installEventFilter(this);

    this->field = new Field();
    this->udp = new Udp(this);
    
    connect(this->udp, &Udp::received, this, &MainWindow::handlePoint);
    
    // QString filename = "/home/yu/MafiaMook/my_image2.jpg";
    // QImage image(filename);

    playchoice[0] = false;   // 처음엔 1번 플레이어 선택 상태
    playchoice[1] = false;
    playchoice[2] = false;
    player=1; //이 컴퓨터는 1번 플레이어

    // if (image.isNull()) {
    //     cout<< "이미지 로드 실패:" <<filename.toStdString()<<endl;
    // } else {
    //     ui->label_2->setPixmap(
    //         QPixmap::fromImage(image).scaled(
    //             ui->label_2->size(),
    //             Qt::KeepAspectRatio,
    //             Qt::SmoothTransformation
    //             )
    //         );
    // }

    board.resize(boardSize);
    for(int i=0; i<boardSize; i++){
        board[i].resize(boardSize);
        for(int j=0; j<boardSize; j++){
            board[i][j] = 0; // 빈칸 초기화
        }
    }

    drawBoard();

    //QWidget *container = new QWidget(this);
    //container->setMinimumSize(1, 1);
    //container->setFixedWidth(ui->scrollArea->width());
    //ui->scrollArea->setWidget(container);
    //ui->scrollArea->setWidgetResizable(false);
    //ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handlePoint(Point p)
{
    //
}

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

QGraphicsEllipseItem* stoneItems[20][20] = {nullptr};  // 최대 20x20 오목판 가정

void MainWindow::placeStone(int row, int col, int player) {
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize || board[row][col] != 0) {
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

    QGraphicsEllipseItem* stone = scene->addEllipse(x, y, stoneSize, stoneSize, QPen(), brush);
    stoneItems[row][col] = stone; // 저장
}

// 돌 제거 함수
void MainWindow::removeStone(int row, int col) {
    if (stoneItems[row][col] != nullptr) {
        scene->removeItem(stoneItems[row][col]);
        delete stoneItems[row][col];   // 메모리 정리
        stoneItems[row][col] = nullptr;
        board[row][col] = 0;
    }
}

void MainWindow::showMafiaEffect(int row, int col, int stoneSize) {


    // 간단한 펄스 효과: QTimer로 크기 변화를 줌
    // QTimer* timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, [effect]() {
    //     static bool growing = true;
    //     qreal scaleFactor = effect->scale();
    //     if (growing) {
    //         scaleFactor += 0.05;
    //         if (scaleFactor >= 1.5) growing = false;
    //     } else {
    //         scaleFactor -= 0.05;
    //         if (scaleFactor <= 1.0) growing = true;
    //     }
    //     effect->setScale(scaleFactor);
    // });
    // timer->start(50);  // 50ms마다
}

void MainWindow::onGraphicsViewClicked(QPointF pos) {
    QPointF cell = mapToCell(pos); //clicked-->포인트 가져오기
    if(playchoice[0]){
        placeStone(cell.x(), cell.y(), player); //player마다 다른 돌
    }
    else if(playchoice[1]){
        if(board[cell.x()][cell.y()]!= player){
            ui->label->setText("Wrong Access");
        }
        else{
            removeStone(cell.x(), cell.y());
            ui->label->setText("Stone Distroied");
        }
    }
    else if(playchoice[2]){
        if(board[cell.x()][cell.y()]!= player){
            ui->label->setText("What are you doing?");
        }
        else{
            ui->label->setText("Stone rehabilitated");
            int stoneSize = cellSize - 4;
            int centerX = cell.x() * cellSize;
            int centerY = cell.y() * cellSize;

            int x = centerX - stoneSize / 2;
            int y = centerY - stoneSize / 2;

            // 여기서 stone 대신 x, y, stoneSize를 이용해서 효과 적용
            qreal stoneSizeF = static_cast<qreal>(stoneSize);
            showMafiaEffect(x, y, stoneSize);
        }
    }
    else{
        return;
    }

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

void MainWindow::on_radioButton_clicked()
{
    playchoice[0]=true;
    playchoice[1]=false;
    playchoice[2]=false;
}


void MainWindow::on_radioButton_2_clicked()
{
    playchoice[0]=false;
    playchoice[1]=true;
    playchoice[2]=false;
}


void MainWindow::on_radioButton_3_clicked()
{
    playchoice[0]=false;
    playchoice[1]=false;
    playchoice[2]=true;
}







