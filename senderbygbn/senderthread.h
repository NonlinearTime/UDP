#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H


#include <QThread>
#include <QtNetwork>
#include <QDebug>
#include <QTimer>

struct ProtocalDatagram {
    quint8 isAck=0; //0:file_packages,1:ack
    quint8 index=0;
    quint16 size=0;
    QByteArray data;
};

inline QDataStream &operator <<(QDataStream &out,const ProtocalDatagram & proDatagram) {
    out << proDatagram.isAck << proDatagram.index << proDatagram.size << proDatagram.data;
    return out;
}

inline QDataStream &operator >>(QDataStream &in,ProtocalDatagram & proDatagram) {
    quint8 isAck,index;
    quint16 size;
    QByteArray data;
    in >> isAck >> index >> size >> data;

    proDatagram.isAck = isAck;
    proDatagram.index = index;
    proDatagram.size = size;
    proDatagram.data = data;
    return in;
}

class SenderThread : public QThread
{
    Q_OBJECT
public:
    explicit SenderThread(ProtocalDatagram * prodata,QHostAddress addr, quint16 port,QObject *parent = 0);

    void run();

signals:
    //void timeToBreak();

public slots:
    void sendEnd();
    void sendData();

private:
    QHostAddress addr;
    quint16 port;
    QUdpSocket *udpSocket;
    ProtocalDatagram *prodata;
    int count;
};

#endif // SENDERTHREAD_H
