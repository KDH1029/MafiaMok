#define IP "192.168.0.37"

#include <QUdpSocket>
#include <iostream>

#include "udp.h"

Udp::Udp(void){
    this->socket = new QUdpSocket(this);
    this->socket->bind(QHostAddress::Any, 9999);
    connect(this->socket, &QUdpSocket::readyRead, this, &MainWindow::get_udp);
}
Udp::~Udp(void){
    delete socket;
}

Point Udp::get_udp()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString message = QString::fromUtf8(datagram);
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

        return {x,y};
    }
}

void Udp::send(QString str)
{
    this->socket->writeDatagram(str.toUtf8(), QHostAddress(IP), 9999);
}