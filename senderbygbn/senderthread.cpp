#include "senderthread.h"
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

SenderThread::SenderThread(ProtocalDatagram * prodata,QHostAddress addr, quint16 port, QObject *parent)
    :QThread(parent)
{
    this->addr = addr;
    this->port = port;
    this->prodata = prodata;
}

void SenderThread::run()
{
    udpSocket = new QUdpSocket();
    udpSocket->connectToHost(addr,port);
    sendData();
    exec();
}

void SenderThread::sendEnd()
{
    qDebug() << "end";

    udpSocket->deleteLater();
    //timer->deleteLater();
    exit(0);
}

void SenderThread::sendData()
{
    //wait(1);
    QByteArray ba;
    ba.resize(sizeof(ProtocalDatagram));

    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    out << *prodata;
    buffer.close();

    udpSocket->write(ba.data(),ba.size());
    //qDebug() << ba;
    qDebug() << "dup" << prodata->index;
    QTimer::singleShot(2000,this,SLOT(sendData()));
}

