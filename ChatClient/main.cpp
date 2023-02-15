#include "mainwindow.h"
#include "dbconnect.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if (create_connection()) w.show();
    return a.exec();
}
