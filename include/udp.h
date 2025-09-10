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
    void pointReceived(Point p);
    void cmdReceived(const QString &cmd);

private slots:
    void get_udp();

private:
    QUdpSocket *socket;
};
