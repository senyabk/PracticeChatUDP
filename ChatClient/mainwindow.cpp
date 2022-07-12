#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "windows.h"
#include "dbconnect.h"

#include <QUdpSocket>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileDevice>

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
    quint16 msgSenderPort; //порт отправителя
    qint16 fileFlag = 0; //проверка передачи файла
    QByteArray fileName;

    while(socket->hasPendingDatagrams()){
        QByteArray datagram, delivered, receivedData, fullData;
        quint64 socketsNumber = 0, buf = 0, msg_id = 0, metaData;
        qint64 fileSize;
        QFile file;
        datagram.resize(socket->pendingDatagramSize());

        //цикл для объединения пакетов в сообщение
        do{
            if (!socket->waitForReadyRead()){ //ожидание следующего пакета
                ui->textEdit->append("Ошибка, пакетов не доставлено: "
                                     + QString::number(socketsNumber - buf)
                                     + " из " + QString::number(socketsNumber));
            }

            socket->readDatagram(datagram.data(), datagram.size(), &msgSender, &msgSenderPort); //чтение датаграммы

//            if (socket->errorString() == "Connection reset by peer"){ //ошибка при отправке
//                ui->textEdit->append("Ошибка передачи");
//                return;
//            }

//            if (datagram.left(6) == "/0del/"){
//                datagram.remove(0, 6);
//                ui->textEdit->append("Сообщение " + QString(datagram) + " доставлено");
//                return;
//            }

            QDataStream stream(datagram);
            stream >> metaData;
            switch (metaData){
                //чтение сообщения
                case 0:
                    stream >> socketsNumber;
                    stream >> msg_id;
                    stream >> receivedData;
                    fullData += receivedData;
                    buf++;
                    break;
                //чтение файла
                case 1:
                    stream >> fileName;
                    stream >> receivedData;
                    stream >> fileSize;
                    file.setFileName(fileName);
                    if(file.open(QFile::Append)) {
                        QTextStream writeStream(&file);
                        writeStream << receivedData;
                        fileFlag = 1;
                        file.close();
                    }
                    break;
                 //чтение сообщения о доставке
                 case 2:
                    stream >> msg_id;

                    ui->textEdit->append("Сообщение " + QString::number(msg_id) + " доставлено");

                    QSqlQuery query;
                    query.prepare("UPDATE messages SET delivery_status = :del_stat WHERE sender_address = '127.0.0.1'"
                                  "AND sender_port = :sp AND message_id = :msg_id");
                    query.bindValue(":del_stat", "Delivered");
                    //query.bindValue(":sender_address", "");
                    query.bindValue(":sp", ui->sender_port->value());
                    query.bindValue(":msg_id", msg_id);
                    query.exec();

                    fileFlag = -1;
                    break;
            }
        }while(buf != socketsNumber); //в случае не с обычными сообщениями они изначально равны

        //если обычное сообщение, вывод на экран + квитанция о доставке
        if (fileFlag == 0){
            quint64 del_meta = 2;
            QDataStream delivered_stream(&delivered, QIODevice::WriteOnly);
            delivered_stream << del_meta;
            delivered_stream << msg_id;

            socket->writeDatagram(delivered, QHostAddress::LocalHost, msgSenderPort);
            Sleep(delay);

            ui->textEdit->append("От " + QString::number(msgSenderPort) + ": " + QString(fullData));
            ui->textEdit->append("Пакетов: " + QString::number(socketsNumber));
        }
    }
    if (fileFlag == 1){
        ui->textEdit_2->append("Получен файл " + fileName);
    }

}

void MainWindow::on_pushButton_clicked() //авторизация (в данном случае, задание хоста)
{
    socket->bind(QHostAddress::LocalHost, ui->sender_port->value());
    ui->checkBox->setChecked(true);
    ui->checkBox->setText("Пользователь: " + ui->sender_port->text());
    msgID = 0;

    //Вывод истории сообщений
    bool flag = false;
    QSqlQuery query;
    query.prepare("SELECT message, message_id, receiver_port FROM messages "
                  "WHERE sender_address = '127.0.0.1' AND sender_port = :sp");
    //query.bindValue(":sender_address",);
    query.bindValue(":sp", ui->sender_port->value());
    query.exec();

    while (query.next()){
        QString message = query.value(0).toString();
        quint64 msg_id = query.value(1).toInt();
        quint64 rec_port = query.value(2).toInt();
        ui->textEdit->append("Вы->" + QString::number(rec_port) + ": " + message + " (" + QString::number(msg_id) + ")");
        if (msg_id > msgID) msgID = msg_id;
        flag = true;
    }

    if (flag) msgID++;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readingData()));
}

void MainWindow::on_sendingButton_clicked() //отправка сообщения
{
// Отправка одним пакетом
//    socket->writeDatagram(ui->msg_text->text().toUtf8(), QHostAddress::LocalHost, ui->receiver_port->value());
//    ui->textEdit->append("Вы: " + ui->msg_text->text());
//    ui->lineEdit->clear();

    QByteArray data = ui->msg_text->text().toUtf8();
    const quint64 fullDataSize = data.size();

    //разделение на пакеты заданного размера
    while (data.size() > 0)
    {
        QByteArray sendingData = data.left(socketSize), datagram;
        quint64 metaData = 0;

        QDataStream stream(&datagram, QIODevice::WriteOnly);
        stream << metaData;
        (fullDataSize % socketSize > 0) ? stream << fullDataSize/socketSize + 1: stream << fullDataSize/socketSize;
        stream << msgID;
        stream << sendingData;

        socket->writeDatagram(datagram, QHostAddress::LocalHost, ui->receiver_port->value());
        Sleep(delay);
        data.remove(0, sendingData.size());
    }

    //Запрос для добавления сообщения в БД
    QSqlQuery query;
    query.prepare("INSERT INTO MESSAGES (sender_port,  receiver_port, "
                                        "message, message_id, delivery_status) "
              "VALUES (:sp, :rp, :msg, :msg_id, :dstat)");
    query.bindValue(":sp", ui->sender_port->value());
    query.bindValue(":rp", ui->receiver_port->value());
    query.bindValue(":msg", ui->msg_text->text());
    query.bindValue(":msg_id", msgID);
    query.bindValue(":dstat", "Send");
    query.exec();

    ui->textEdit->append("Вы: " + ui->msg_text->text() + " (" + QString::number(msgID) + ")");
    msgID++;
    ui->msg_text->clear();
}


void MainWindow::on_sizeButton_clicked() //установка размера пакетов
{
    socketSize = ui->sizeSpinBox->value();
    ui->checkBox_2->setChecked(true);
    ui->checkBox_2->setText("Пакет: " + QString::number(socketSize) + " байт");
}


void MainWindow::on_delayButton_clicked() //установка задержки
{
    delay = ui->delaySpinBox->value();
    ui->checkBox_3->setChecked(true);
    ui->checkBox_3->setText("Задержка: " + QString::number(delay) + " мс");
}


void MainWindow::on_sendFile_clicked() //отправка файла
{
    QString fileName = QFileDialog::getOpenFileName(this, "File send", "C://");
    QFile file(fileName);
    if(file.open(QFile::ReadOnly)) {
            qint64 raw_size = 0;
            char raw_data[socketSize];
            quint64 metaData = 1;
            while((raw_size = file.read(raw_data, socketSize)) > 0) {
                QByteArray sendingData = QByteArray::fromRawData(raw_data, raw_size), datagram;

                QDataStream stream(&datagram, QIODevice::WriteOnly);
                stream << metaData;
                stream << fileName.toUtf8();
                stream << sendingData;
                stream << file.size();
                socket->writeDatagram(datagram, QHostAddress::LocalHost, ui->receiver_port->value());
            }
        }
    if (!fileName.isEmpty()){
        QMessageBox::information(this, "Info", "Файл " + fileName + " отправлен");
    }
}
