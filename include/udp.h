#include <QUdpSocket>

struct Point {
    int x;
    int y;
};

class UDP
{
public:
    Socket *socket;
    UDP();
    ~UDP();

private slots:
    Point get_udp();

private:
    QUdpSocket *udpSocket;

    void send(QString str);
}

