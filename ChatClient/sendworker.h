#ifndef SENDWORKER_H
#define SENDWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QDataStream>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include "windows.h"
#include "dbconnect.h"

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

public slots:
    void runSending(QByteArray data, quint64 msgID, QUdpSocket &socket, quint64 socketSize, quint64 recPort, quint64 delay);

signals:



    // QThread interface
protected:
    void run();
};

#endif // SENDWORKER_H
