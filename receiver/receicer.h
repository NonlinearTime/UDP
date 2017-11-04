#ifndef RECEICER_H
#define RECEICER_H

#include <QObject>
#include <QtNetwork>
#include <QFile>
#include <QHash>
#include <QQueue>
#include <QPair>
#include <receivedfile.h>

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

class Receicer : public QObject
{
    Q_OBJECT
public:
    explicit Receicer(QObject *parent = nullptr);
    QHostAddress addr;
    quint16 port;
    int lineLen;
    int groupNum;
    int windowSize;
    int head,tail;
    int sendSize;
    QFile file;
    QByteArray buff;

signals:
    void sendedSize(qint64 size);

public slots:
    void readPendingDatagrams();
    void initContainer();
    void moveForward();
    void sendAck(const ProtocalDatagram *ack);
    void sendFileSize();

private:
    QUdpSocket *udpSocket;
    QUdpSocket *sendUdpSocket;
    void initSocket();

    QQueue<QPair<bool,ProtocalDatagram*>*> ackedRec;
};

#endif // RECEICER_H
