#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "windows.h"
#include "sendworker.h"

#include <QUdpSocket>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileDevice>
#include <QMainWindow>
#include <QSqlQuery>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QUdpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readingData();

signals:
    void startSending(QByteArray data, quint64 msgID, QUdpSocket &socket, quint64 socketSize, quint64 recPort, quint64 delay);

private slots:
    void on_pushButton_clicked();

    void on_sendingButton_clicked();

    void on_sizeButton_clicked();

    void on_delayButton_clicked();

    void on_sendFile_clicked();

    void on_msg_text_returnPressed();

    void on_requestButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    SendWorker *sendWorker;
    QThread *sendThread;
    quint64 socketSize;
    quint64 delay;
    quint64 reqID;


};
#endif // MAINWINDOW_H
