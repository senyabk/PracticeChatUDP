#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->msg_text->setReadOnly(true);
    ui->sendingButton->setDisabled(true);
    ui->requestButton->setDisabled(true);

    //sendThread = new QThread(this);
    //connect(this, SIGNAL(destroyed()), sendThread, SLOT(quit()));

    // Создание объекта для потока
    sendWorker = new SendWorker();
    connect(this, SIGNAL(destroyed()), sendWorker, SLOT(quit()));

    fileSendWorker = new FileSendWorker();
    connect(this, SIGNAL(destroyed()), fileSendWorker, SLOT(quit()));

    socket = new QUdpSocket(this); //init socket

    //connect(this, SIGNAL(startSending(QByteArray,quint64,QUdpSocket&,quint64,quint64,quint64)),
      //      sendWorker, SLOT(runSending(QByteArray,quint64,QUdpSocket&,quint64,quint64,quint64)));

    // Перенос объекта в свой поток
    sendWorker->moveToThread(sendWorker);
    fileSendWorker->moveToThread(fileSendWorker);

    //sendWorker->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::readingData() // чтение сообщения (файла, квитанции о доставке)
{
    QHostAddress msgSender; //aдрес отправителя
    quint16 msgSenderPort; //порт отправителя
    qint16 fileFlag = 0; //проверка передачи файла
    quint64 msg_id;
    QByteArray fileName, fileSuffix, delivered, fullData;

    while(socket->hasPendingDatagrams()){
        QByteArray datagram, receivedData;
        quint64 socketsNumber = 0, buf = 0, metaData;
        qint64 fileSize;
        QFile file;
        datagram.resize(socket->pendingDatagramSize());

        //цикл для объединения пакетов в сообщение
        do{
            if (!socket->waitForReadyRead()){ //ожидание следующего пакета
                ui->textEdit->append("Ошибка, пакетов не доставлено: "
                                     + QString::number(socketsNumber - buf)
                                     + " из " + QString::number(socketsNumber));

                 ui->requestButton->setEnabled(true); //при ошибке сообщение запрашивается из БД
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
                    reqID = msg_id;
                    buf++;
                    break;
                //чтение файла
                case 1:
                    stream >> fileName;
                    stream >> fileSuffix;
                    stream >> receivedData;
                    stream >> fileSize;
                    file.setFileName(fileName + "." + fileSuffix);
                    if(file.open(QFile::Append)) {
                        QTextStream writeStream(&file);
                        writeStream << receivedData;
                        fileFlag = 1;
                        file.close();
                    }
                    break;
                 //чтение сообщения о доставке
                case 2:
                    stream >> fileName;

                    ui->textEdit->append("Файл " + fileName + " доставлен");

                    fileFlag = -1;
                    break;
                case 3:
                    stream >> msg_id;

                    ui->textEdit->append("Сообщение " + QString::number(msg_id) + " доставлено");
                    QSqlQuery query;
                    query.prepare("UPDATE messages SET delivery_status = :del_stat WHERE id = :msg_id");
                    query.bindValue(":del_stat", "Delivered");
                    query.bindValue(":msg_id", msg_id);
                    query.exec();

                    fileFlag = -1;
                    break;


            }
        }while(buf != socketsNumber); //в случае не с обычными сообщениями они изначально равны


    }
    if (fileFlag == 0){
        quint64 del_meta = 3;
        QDataStream delivered_stream(&delivered, QIODevice::WriteOnly);
        delivered_stream << del_meta;
        delivered_stream << msg_id;

        ui->textEdit->append("От " + QString::number(msgSenderPort) + ": " + QString(fullData));
        //ui->textEdit->append("Пакетов: " + QString::number(socketsNumber));

        socket->writeDatagram(delivered, QHostAddress::LocalHost, msgSenderPort);
        Sleep(delay);

    }else if (fileFlag == 1){
        quint64 del_meta = 2;
        QDataStream delivered_stream(&delivered, QIODevice::WriteOnly);
        delivered_stream << del_meta;
        delivered_stream << fileName + "." + fileSuffix;

        ui->textEdit->append("Получен файл " + fileName + "." + fileSuffix);

        socket->writeDatagram(delivered, QHostAddress::LocalHost, msgSenderPort);
        Sleep(delay);

    }

}

void MainWindow::on_pushButton_clicked() // авторизация (в данном случае, задание хоста)
{
    socket->bind(QHostAddress::LocalHost, ui->sender_port->value());
    ui->checkBox->setChecked(true);
    ui->checkBox->setText("Пользователь: " + ui->sender_port->text());

    //Вывод истории сообщений
    QSqlQuery query;
    query.prepare("SELECT id, message, sender_port, receiver_port, delivery_status FROM messages "
                  "WHERE sender_address = '127.0.0.1' AND (sender_port = :sp OR receiver_port = :sp)");
    //query.bindValue(":sender_address",);
    query.bindValue(":sp", ui->sender_port->value());
    query.exec();

    while (query.next()){
        quint64 msg_id = query.value(0).toInt();
        QString message = query.value(1).toString();
        quint64 send_port = query.value(2).toInt();
        quint64 rec_port = query.value(3).toInt();
        QString status = query.value(4).toString();
        if (rec_port == ui->sender_port->value() && status == "Delivered"){
            ui->textEdit->append("От " + QString::number(send_port) + ": " + message);
        }else if (send_port == ui->sender_port->value()){
            ui->textEdit->append("Вы->" + QString::number(rec_port) + ": " + message + " (" + QString::number(msg_id) + " " + status + ")");

        }
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readingData()));

    ui->msg_text->setReadOnly(false);
    ui->sendingButton->setDisabled(false);
}

void MainWindow::on_sendingButton_clicked() // отправка сообщения по нажатию кнопки
{
// Отправка одним пакетом
//    socket->writeDatagram(ui->msg_text->text().toUtf8(), QHostAddress::LocalHost, ui->receiver_port->value());
//    ui->textEdit->append("Вы: " + ui->msg_text->text());
//    ui->lineEdit->clear();

    QByteArray data = ui->msg_text->text().toUtf8();

    //Запрос для добавления сообщения в БД
    QSqlQuery query;
    query.prepare("INSERT INTO MESSAGES (sender_port,  receiver_port, "
                                        "message, delivery_status) "
              "VALUES (:sp, :rp, :msg, :dstat) RETURNING id");
    query.bindValue(":sp", ui->sender_port->value());
    query.bindValue(":rp", ui->receiver_port->value());
    query.bindValue(":msg", ui->msg_text->text());
    query.bindValue(":dstat", "Created");
    query.exec();
    query.next();

    quint64 msgID = query.value(0).toInt();

    //emit startSending(data, msgID, *socket, socketSize, ui->receiver_port->value(), delay);

    qDebug() << QThread::currentThreadId();;

    // Запуск потока
    sendWorker->data = data;
    sendWorker->msgID = msgID;
    sendWorker->socket = socket;
    sendWorker->socketSize = socketSize;
    sendWorker->recPort = ui->receiver_port->value();
    sendWorker->delay = delay;
    sendWorker->start();


    query.prepare("UPDATE messages SET delivery_status = :del_stat WHERE id = :msg_id");
    query.bindValue(":del_stat", "Send");
    query.bindValue(":msg_id", msgID);
    query.exec();

    ui->textEdit->append("Вы: " + ui->msg_text->text() + " (id: " + QString::number(msgID) + ")");
    ui->msg_text->clear();
}

void MainWindow::on_sizeButton_clicked() // установка размера пакетов
{
    socketSize = ui->sizeSpinBox->value();
    ui->checkBox_2->setChecked(true);
    ui->checkBox_2->setText("Пакет: " + QString::number(socketSize) + " байт");
}

void MainWindow::on_delayButton_clicked() // установка задержки
{
    delay = ui->delaySpinBox->value();
    ui->checkBox_3->setChecked(true);
    ui->checkBox_3->setText("Задержка: " + QString::number(delay) + " мс");
}

void MainWindow::on_sendFile_clicked() // отправка файла
{
    QString fileName = QFileDialog::getOpenFileName(this, "File send", "C://");

    qDebug() << QThread::currentThreadId();;

    // Запуск потока
    fileSendWorker->fileName = fileName;
    fileSendWorker->socketSize = socketSize;
    fileSendWorker->socket = socket;
    fileSendWorker->recPort = ui->receiver_port->value();
    fileSendWorker->start();


    if (!fileName.isEmpty()){
        QMessageBox::information(this, "Info", "Файл " + fileName + " отправлен");
    }
}

void MainWindow::on_msg_text_returnPressed() // отправка сообщению по Enter
{
    ui->sendingButton->click();
}

void MainWindow::on_requestButton_clicked()
{
    //Вывод истории сообщений
    QSqlQuery query;
    query.prepare("SELECT message, sender_port FROM messages "
                  "WHERE id = :reqID AND delivery_status = :del_stat");
    query.bindValue(":reqID", reqID);
    query.bindValue(":del_stat", "Send");
    query.exec();

    if (query.next())
    {
        QString message = query.value(0).toString();
        quint64 send_port = query.value(1).toInt();

        ui->textEdit->append("От " + QString::number(send_port) + ": " + message);

        query.prepare("UPDATE messages SET delivery_status = :del_stat WHERE id = :msg_id");
        query.bindValue(":del_stat", "Delivered");
        query.bindValue(":msg_id", reqID);
        query.exec();
    } else
    {
        QMessageBox::information(this, "Info", "Не удалось дозапросить");
    }

    ui->requestButton->setDisabled(true);

}


