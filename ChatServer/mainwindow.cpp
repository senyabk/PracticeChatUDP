#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QUdpSocket(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_enter_clicked()
{
    auto datagram = ui->msg->text().toLatin1();
    socket->writeDatagram(datagram, QHostAddress::Broadcast, ui->port->value());
}



