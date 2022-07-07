#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QUdpSocket(this);
    connect(socket, &QUdpSocket::readyRead, [&] (){
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size());
        ui->listWidget->addItem(QString(datagram));

    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    socket->bind(ui->spinBox->value(), QUdpSocket::ShareAddress);
}


