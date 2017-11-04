#include "receicer.h"
#include <QDebug>
#include <QTimer>

Receicer::Receicer(QObject *parent) : QObject(parent)
{
    lineLen = 0;
    initSocket();
    windowSize = 30; //best at 30 where Mss = 12 about 10M per sec
    groupNum = 60000;
    head = 0;
    sendSize = 0;
    tail = head + windowSize - 1;
    sendUdpSocket = new QUdpSocket(this);
}

void Receicer::readPendingDatagrams()
{
    qDebug() << "reading";
    while(udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size(),&addr,&port);
        QBuffer buffer(&datagram);
        buffer.open(QIODevice::ReadOnly);

        ProtocalDatagram *proDatagram = new ProtocalDatagram;
        //proDatagram.size = udpSocket->pendingDatagramSize();
        proDatagram->data.resize(proDatagram->size-4);
        QDataStream in(&buffer);
        in >> *proDatagram;
        buffer.close();
        qDebug() << "index" << proDatagram->index << ", size" << proDatagram->size <<", isAck?" << proDatagram->isAck;

        if (proDatagram->isAck == 0) {
            ProtocalDatagram *ack = new ProtocalDatagram;
            ack->index = proDatagram->index;
            ack->isAck = true;
            ack->size = head;
            if (head < tail) {
                if (head <= proDatagram->index && proDatagram->index <= tail) {
                    sendAck(ack);
                    qDebug() << "ack1" <<ack->index;
                    ackedRec[proDatagram->index]->first = true;
                    ackedRec[proDatagram->index]->second = proDatagram;
                    while (ackedRec[head]->first) {
                        qDebug() << ackedRec.first()->first;
                        file.write(ackedRec[head]->second->data);
                        //buff.append(ackedRec[head]->second->data);
                        //sendSize += ackedRec[head]->second->data.size();
                        //sendSize += 12 * 128 - 5;
                        ReceivedFile::GetInstance()->fileSize += ackedRec[head]->second->data.size();
                        //emit sendedSize(ackedRec[head]->second->data.size());
                        //emit sendedSize(12 * 128 - 5);
                        moveForward();
                    }
                } else {
                    int index = proDatagram->index < head ? proDatagram->index + groupNum : proDatagram->index;
                    if (index >= head - windowSize + groupNum && index < head + groupNum) {
                        sendAck(ack);
                        qDebug() << "ack2" <<ack->index;
                    }
                }
            } else {
                if (head - windowSize <= proDatagram->index && proDatagram->index < head ) {
                    sendAck(ack);
                    qDebug() << "ack3" <<ack->index;
                } else if (proDatagram->index >= head || proDatagram->index <= tail) {
                    sendAck(ack);
                    qDebug() << "ack4" <<ack->index;
                    ackedRec[proDatagram->index]->first = true;
                    ackedRec[proDatagram->index]->second = proDatagram;
                    while (ackedRec[head]->first) {
                        qDebug() << ackedRec.first()->first;
                        file.write(ackedRec[head]->second->data);
                        //buff.append(ackedRec[head]->second->data);
                        //sendSize += ackedRec[head]->second->data.size();
                        //sendSize += 12*128 - 5;
                        ReceivedFile::GetInstance()->fileSize += ackedRec[head]->second->data.size();
                        //emit sendedSize(ackedRec[head]->second->data.size());
                        //emit sendedSize(12 * 128 -5);
                        moveForward();
                    }
                }
            }
            delete ack;
        }
    }
}

void Receicer::initContainer()
{
    lineLen=0;
    head = 0 ;
    tail = head + windowSize - 1;
    ackedRec.clear();
    for (int i = 0 ; i != groupNum ; i++) {
        QPair<bool,ProtocalDatagram*> *pair = new QPair<bool,ProtocalDatagram*>(false,nullptr);
        ackedRec.push_back(pair);
    }
//udpSocket->reset()
    //udpSocket->disconnectFromHost();
    //udpSocket->connectToHost(addr,port);
}

void Receicer::moveForward()
{
    qDebug() << "before" << ackedRec.size() << head << tail;

    ackedRec[head]->first = false;
    delete ackedRec[head]->second;
    ackedRec[head]->second = nullptr;
    if (++head == groupNum) head = 0;
    if (++tail == groupNum) tail = 0;

    qDebug() << "after"  << ackedRec.size() << head << tail;
    //if (++lineLen == 1000) {
    //    emit sendedSize(sendSize);
    //    sendSize = 0;
    //    lineLen =0;
    //}

}

void Receicer::sendAck(const ProtocalDatagram *ack)
{
    //ProtocalDatagram mack = ack;
    //qDebug() << mack.index << mack.isAck;
    QByteArray ba;
    ba.resize(sizeof(ProtocalDatagram));

    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);

    QDataStream out(&buf);
    out << *ack;
    buf.close();
    //qDebug() << addr << port;
    udpSocket->writeDatagram(ba.data(),ba.size(),addr,7000);

}

void Receicer::sendFileSize()
{
    emit sendedSize(sendSize);
    sendSize = 0;
    //if(udpSocket->bytesAvailable()!=0) {
    //    qDebug() << "socket failes, restart!";
    //    udpSocket->close();
    //    initSocket();
    //}
    //QTimer::singleShot(1000,this,SLOT(sendFileSize()));
}

void Receicer::initSocket()
{
    udpSocket = new QUdpSocket(this);
    sendUdpSocket = new QUdpSocket(this);
    udpSocket->bind(7001,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    //udpSocket->setReadBufferSize(1024 * 1024);
    qDebug() << udpSocket->readBufferSize();
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()),Qt::DirectConnection);


    if(file.isOpen())
        return;
    file.setFileName("filegotten.apk");
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Unbuffered))
        return;
}

