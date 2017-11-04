#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include <QObject>
#include <QtNetwork>
#include <QRunnable>

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

class sendThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit sendThread(ProtocalDatagram proDatagram,QHostAddress addr, quint16 port, QObject *parent = nullptr);
    ProtocalDatagram prodata;

signals:

public slots:

protected:
    void run();

private:
    void sendData();
    int count;

    QHostAddress addr;
    quint16 port;
    QUdpSocket *udpSocket;
};

#endif // SENDTHREAD_H
