#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QFile>
#include <QtNetwork>
#include <QDataStream>
#include <QBuffer>
#include <QHash>
#include <QQueue>
#include <QPair>
//#include <sendthread.h>
#include <QThreadPool>
#include <QVector>
//#include <senderthread.h>
#include <readthread.h>
#include <sendedfile.h>

/*struct ProtocalDatagram {
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
}*/

class Sender : public QObject
{
    Q_OBJECT
public:
    explicit Sender(QObject *parent = nullptr);
    void sendData(const ProtocalDatagram & proDatagram);
    void sendFile();
    QHostAddress addr;
    quint16 port;
    int MssLen; //单位kb
    int groupNum;
    int windowSize;
    int head,tail;

signals:
    void sendFileSize(quint64 size);
    void sendFileMaxSize(quint64 size);
    void passProdata(const ProtocalDatagram & prodata);

public slots:
    void getFileName(const QString &fileName);
    void getIpAddress(QHostAddress ipAddr,quint16 port);
    void readPendingDatagrams();
    void timeoutResend();
    void sendToThread(const ProtocalDatagram & prodata);

private:
    void initSocket();
    QUdpSocket *udpSocket;
    QFile file;
    int lineLen;
    int lineSended;
    quint64 fileSize;
    int threadIndex;
    int NotReceiveNum;
    int multiAck[100000];
    //QHash<quint8,QPair<ProtocalDatagram *,int>> packages;
    //QHash<quint8,SenderThread *> threadpool;
    QQueue<QPair<bool,ProtocalDatagram *>*> ackedRec;
    ProtocalDatagram* readFile();
    QTimer *timer;
    //QVector<SenderThread*> threadpool;
};

#endif // SENDER_H
