#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QUdpSocket(this); //init socket
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::readingData()
{
    QHostAddress msgSender;
    quint16 msgSenderPort;

    while(socket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size(), &msgSender, &msgSenderPort);
        ui->textEdit->append(QString(datagram));
    }


}

void MainWindow::on_pushButton_clicked()
{
    socket->bind(QHostAddress::LocalHost, ui->sender_port->value());
    connect(socket,SIGNAL(readyRead()),this,SLOT(readingData()));
}

void MainWindow::on_sendingButton_clicked()
{
    socket->writeDatagram(ui->lineEdit_3->text().toUtf8(), QHostAddress::LocalHost, ui->receiver_port->value());
    ui->textEdit->append("Вы: " + ui->lineEdit_3->text());
    ui->lineEdit->clear();
}

