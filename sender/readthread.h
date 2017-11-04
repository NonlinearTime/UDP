#ifndef READTHREAD_H
#define READTHREAD_H

#include <QObject>
#include <QThread>
#include <QtNetwork>


struct ProtocalDatagram {
    quint8 isAck=0; //0:file_packages,1:ack
    quint16 index=0;
    quint16 size=0;
    QByteArray data;
};

inline QDataStream &operator <<(QDataStream &out,const ProtocalDatagram & proDatagram) {
    out << proDatagram.isAck << proDatagram.index << proDatagram.size << proDatagram.data;
    return out;
}

inline QDataStream &operator >>(QDataStream &in,ProtocalDatagram & proDatagram) {
    quint8 isAck;
    quint16 index,size;
    QByteArray data;
    in >> isAck >> index >> size >> data;

    proDatagram.isAck = isAck;
    proDatagram.index = index;
    proDatagram.size = size;
    proDatagram.data = data;
    return in;
}

class ReadThread : public QThread
{
    Q_OBJECT
public:
    ReadThread();

};

#endif // READTHREAD_H
