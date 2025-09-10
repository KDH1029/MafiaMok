#include <QObject>
#include <QUdpSocket>

struct Point
{
    int x;
    int y;
    int value;
};

class Udp : public QObject
{
    Q_OBJECT
public:
    explicit Udp(QObject *parent = nullptr);
    ~Udp();

    void send(const QString &str);

signals:
    void received(Point p);

private slots:
    void get_udp();

private:
    QUdpSocket *socket;
};
