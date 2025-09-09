#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QUdpSocket>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 9999);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::get_udp);

    QWidget *container = new QWidget(this);
    container->setMinimumSize(1, 1);
    container->setFixedWidth(ui->scrollArea->width());
    ui->scrollArea->setWidget(container);
    ui->scrollArea->setWidgetResizable(false);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
        addBubble(message, false);
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString message = ui->textEdit->toPlainText();
    if (message.isEmpty())
        return;

    udpSocket->writeDatagram(message.toUtf8(), QHostAddress("192.168.0.37"), 9999);
    addBubble(message, true);
    ui->textEdit->clear();
}

void MainWindow::addBubble(const QString &message, bool isSender)
{
    QLabel *label = new QLabel(message);
    label->setWordWrap(true);
    label->setMaximumWidth(ui->scrollArea->width() - 60);
    label->adjustSize();
    label->setStyleSheet(isSender ? "background-color: #FFFFFF; border-radius: 10px; padding: 8px;" : "background-color: #FEE500; border-radius: 10px; padding: 8px;");
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(10, 5, 10, 5);

    if (isSender)
    {
        layout->addStretch();
        layout->addWidget(label);
    }
    else
    {
        layout->addWidget(label);
        layout->addStretch();
    }

    QWidget *bubbleWidget = new QWidget;
    bubbleWidget->setLayout(layout);

    QWidget *container = ui->scrollArea->widget();
    bubbleWidget->setParent(container);
    bubbleWidget->show();

    int yOffset = 0;
    for (QObject *child : container->children())
    {
        QWidget *w = qobject_cast<QWidget *>(child);
        if (w && w != bubbleWidget)
        {
            yOffset += w->height() + 10;
        }
    }

    int containerWidth = container->width();
    int bubbleWidth = bubbleWidget->sizeHint().width();
    int xOffset = isSender ? containerWidth - bubbleWidth - 10 : 10;

    bubbleWidget->move(xOffset, yOffset);
    container->resize(container->width(), yOffset + bubbleWidget->height() + 20);
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}