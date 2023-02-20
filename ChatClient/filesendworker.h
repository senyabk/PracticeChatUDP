#ifndef FILESENDWORKER_H
#define FILESENDWORKER_H

#include <QFile>
#include <QUdpSocket>
#include <QFileInfo>
#include <QDataStream>
#include <QThread>
#include <QObject>
#include <QDebug>
#include "windows.h"

class FileSendWorker : public QThread
{
    Q_OBJECT
public:
    explicit FileSendWorker(QObject *parent = nullptr);
    QString fileName;
    quint64 socketSize;
    QUdpSocket *socket;
    quint64 recPort;

    void runFileSending(QString fileName, quint64 socketSize, QUdpSocket &socket, quint64 recPort);

protected:
    void run();
};

#endif // FILESENDWORKER_H
