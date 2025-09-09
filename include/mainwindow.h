#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void addBubble(const QString &message, bool isSender);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void get_udp();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
};

#endif // MAINWINDOW_H
