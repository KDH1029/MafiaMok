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

// void MainWindow::on_pushButton_clicked()
// {
//     udpSocket->writeDatagram(message.toUtf8(), QHostAddress("192.168.0.37"), 9999);
//     addBubble(message, true);
//     ui->textEdit->clear();
// }