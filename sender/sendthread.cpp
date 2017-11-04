#include "sendthread.h"
#include <QTimer>

sendThread::sendThread(ProtocalDatagram proDatagram, QHostAddress addr, quint16 port, QObject *parent )
{
    prodata = proDatagram;
    this->addr = addr;
    this->port = port;
    udpSocket = new QUdpSocket(this);
    count = 0;
}

void sendThread::run()
{
    udpSocket = new QUdpSocket();
    udpSocket->connectToHost(addr,port);
    sendData();
}

void sendThread::sendData()
{
    //wait(1);
    QByteArray ba;
    ba.resize(sizeof(ProtocalDatagram));

    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    out << prodata;
    buffer.close();

    udpSocket->write(ba.data(),ba.size());
    //qDebug() << ba;
    qDebug() << "dup" << count++;
}
