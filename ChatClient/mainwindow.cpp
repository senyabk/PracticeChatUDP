#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtMath>
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


void MainWindow::readingData() //чтение сообщения
{
    QHostAddress msgSender; //aдрес отправителя
    quint16 msgSenderPort;

    while(socket->hasPendingDatagrams()){
        if (socket->errorString() == "Connection reset by peer"){
            ui->textEdit->append("Ошибка, неверный адресат");
            return;
        }
        QByteArray datagram, receivedData, fullData;
        quint64 socketsNumber = 0, buf = 0;
        datagram.resize(socket->pendingDatagramSize());
        //цикл для объединения пакетов в сообщение
        do{
            socket->readDatagram(datagram.data(), datagram.size(), &msgSender, &msgSenderPort);
            QDataStream stream(datagram);
            stream >> socketsNumber;
            stream >> receivedData;
            fullData += receivedData;
            buf++;
        }while(socket->hasPendingDatagrams());

        //проверка получения всех пакетов
        if (buf != socketsNumber){
            ui->textEdit->append("Ошибка, пакетов не доставлено: "
                                 + QString::number(socketsNumber - buf)
                                 + " из " + QString::number(socketsNumber));
        }
        else{
            ui->textEdit->append("От " + QString::number(msgSenderPort) + ": " + QString(fullData));
            ui->textEdit->append("Пакетов: " + QString::number(socketsNumber));
        }
    }

}

void MainWindow::on_pushButton_clicked() //авторизация (в данном случае, задание хоста)
{
    socket->bind(QHostAddress::LocalHost, ui->sender_port->value());
    ui->checkBox->setChecked(true);
    ui->checkBox->setText("Пользователь: " + ui->sender_port->text());
    connect(socket,SIGNAL(readyRead()),this,SLOT(readingData()));
}

void MainWindow::on_sendingButton_clicked() //отправка сообщения
{

//    socket->writeDatagram(ui->msg_text->text().toUtf8(), QHostAddress::LocalHost, ui->receiver_port->value());
//    ui->textEdit->append("Вы: " + ui->msg_text->text());
//    ui->lineEdit->clear();

    QByteArray data = ui->msg_text->text().toUtf8();
    const quint64 fullDataSize = data.size();

    //разделение на пакеты заданного размера
    while (data.size() > 0)
    {
        QByteArray sendingData = data.left(socketSize);
        QByteArray datagram;
        QDataStream stream(&datagram, QIODevice::WriteOnly);
        (fullDataSize % socketSize > 0) ? stream << fullDataSize/socketSize + 1: stream << fullDataSize/socketSize;
        stream << sendingData;

        socket->writeDatagram(datagram, QHostAddress::LocalHost, ui->receiver_port->value());

        data.remove(0, sendingData.size());
    }

    ui->textEdit->append("Вы: " + ui->msg_text->text());
    ui->msg_text->clear();

}


void MainWindow::on_sizeButton_clicked()
{
    socketSize = ui->sizeSpinBox->value();
    ui->checkBox_2->setChecked(true);
    ui->checkBox_2->setText("Пакет: " + QString::number(socketSize) + " байт");
}

