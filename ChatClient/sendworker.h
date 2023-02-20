#ifndef SENDWORKER_H
#define SENDWORKER_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <QDataStream>
#include <QMessageBox>
#include <QDebug>
#include "windows.h"

class SendWorker : public QThread
{
    Q_OBJECT
public:
    explicit SendWorker(QObject *parent = nullptr);
    QByteArray data;
    quint64 msgID;
    QUdpSocket *socket;
    quint64 socketSize;
    quint64 recPort;
    quint64 delay;

    void runSending(QByteArray data, quint64 msgID, QUdpSocket &socket, quint64 socketSize, quint64 recPort, quint64 delay);

protected:
    void run();
};

#endif // SENDWORKER_H
