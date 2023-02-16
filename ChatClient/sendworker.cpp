#include "sendworker.h"

SendWorker::SendWorker(QObject *parent)
    : QThread{parent}
{

}

void SendWorker::runSending(QByteArray data, quint64 msgID, QUdpSocket &socket, quint64 socketSize, quint64 recPort, quint64 delay)
{
    const quint64 fullDataSize = data.size();
    qDebug() << QThread::currentThreadId();
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

        socket.writeDatagram(datagram, QHostAddress::LocalHost, recPort);
        Sleep(delay);
        data.remove(0, sendingData.size());
    }

}

void SendWorker::run()
{
    runSending(data, msgID, *socket, socketSize, recPort, delay);
}
