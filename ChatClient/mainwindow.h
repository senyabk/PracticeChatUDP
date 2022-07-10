#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void on_pushButton_clicked();

    void on_sendingButton_clicked();

    void on_sizeButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    quint64 socketSize;
};
#endif // MAINWINDOW_H
