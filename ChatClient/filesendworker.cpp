#include "filesendworker.h"

FileSendWorker::FileSendWorker(QObject *parent)
    : QThread{parent}
{

}


void FileSendWorker::runFileSending(QString fileName, quint64 socketSize, QUdpSocket &socket, quint64 recPort)
{
    qDebug() << QThread::currentThreadId();

    QFile file(fileName);
    QFileInfo fi(fileName);
    if(file.open(QFile::ReadOnly)) {
            qint64 raw_size = 0;
            char raw_data[socketSize];
            quint64 metaData = 1;
            while((raw_size = file.read(raw_data, socketSize)) > 0) {
                QByteArray sendingData = QByteArray::fromRawData(raw_data, raw_size), datagram;

                QDataStream stream(&datagram, QIODevice::WriteOnly);
                stream << metaData;
                stream << fi.baseName().toUtf8();
                stream << fi.completeSuffix().toUtf8();
                stream << sendingData;
                stream << file.size();

                socket.writeDatagram(datagram, QHostAddress::LocalHost, recPort);
            }
        }
}

void FileSendWorker::run()
{
    runFileSending(fileName, socketSize, *socket, recPort);
}
