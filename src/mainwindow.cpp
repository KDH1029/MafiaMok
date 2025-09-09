#define IP "192.168.0.37"

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QUdpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 9999);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::get_udp);

    this->field = new Field();
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
        }
    }
}
