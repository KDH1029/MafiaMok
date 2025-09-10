
#include "setting.h"

#if Player
#define IP "192.168.0.37"
#else
#define IP "192.168.0.45"
#endif

#define PORT 9999

#include "udp.h"
#include <QHostAddress>

Udp::Udp(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, PORT);
    connect(socket, &QUdpSocket::readyRead, this, &Udp::get_udp);
}

Udp::~Udp()
{
}

void Udp::get_udp()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(int(socket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString message = QString::fromUtf8(datagram);
        QStringList arr = message.split(",");

        if (arr.size() == 3)
        {
            emit pointReceived(Point{arr[0].toInt(), arr[1].toInt(), arr[2].toInt()});
        }
        if (arr.size() == 1){
            emit cmdReceived(message);
        }
    }
}

void Udp::send(const QString &str)
{
    socket->writeDatagram(str.toUtf8(), QHostAddress(IP), PORT);
}
