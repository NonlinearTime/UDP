#include "sender.h"
#include <QTimer>

Sender::Sender(QObject *parent) : QObject(parent)
{
    MssLen = 8;
    lineLen = 0;
    lineSended = 0;
    threadIndex = 0;
    groupNum = 60000;
    windowSize = 10; //best at 30 where Mss = 12 about 10M per sec
    NotReceiveNum = 0;
    fileSize = 0;
    head = 0 ;
    tail = head + windowSize - 1;
    initSocket();
    connect(this,SIGNAL(passProdata(ProtocalDatagram)),this,SLOT(sendToThread(ProtocalDatagram)));
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeoutResend()));
}

void Sender::sendData(const ProtocalDatagram &proDatagram)
{
    QByteArray ba;
    ba.resize(sizeof(ProtocalDatagram));

    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    out << proDatagram;
    buffer.close();

    udpSocket->writeDatagram(ba.data(),ba.size(),addr,port);
}

void Sender::sendFile()
{
    for (int i = 0 ; i != groupNum ; i++) {
        if (i < windowSize){
            auto m_pDatagram = readFile();
            QPair<bool,ProtocalDatagram*> *pair = new QPair<bool,ProtocalDatagram*>(false,m_pDatagram);
            sendData(*m_pDatagram);
            //SenderThread *thread = new SenderThread(m_pDatagram,addr,port,this);
            //connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
            //thread->start();
            //threadpool.insert(m_pDatagram->index,thread);
            ackedRec.push_back(pair);
        } else {
            QPair<bool,ProtocalDatagram*> *pair = new QPair<bool,ProtocalDatagram*>(false,NULL);
            ackedRec.push_back(pair);
        }
        multiAck[i] = 0;
    }
    QTimer::singleShot(1000,this,SLOT(timeoutResend()));
}

void Sender::initSocket()
{
    udpSocket = new QUdpSocket(this);
    //udpSocket->setReadBufferSize(1024 * 1024);
    udpSocket->bind(7000,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    QMetaObject::Connection con = connect(udpSocket,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()),Qt::DirectConnection);
    qDebug() << con;
}

ProtocalDatagram *Sender::readFile()
{
    if (!file.atEnd()) {
        QByteArray line = file.read(MssLen * 128 - 5);
        ProtocalDatagram *proDatagram = new ProtocalDatagram;
        proDatagram->isAck = 0;
        proDatagram->index = lineLen;
        proDatagram->size = line.size() + 5;
        proDatagram->data.resize(line.size());
        proDatagram->data = line;

        //qDebug() << lineLen %groupNum;
        //fileSize += MssLen * 128 - 5;
        SendedFile::GetInstance()->fileSize += line.size();
        NotReceiveNum++;
        if(++lineLen == groupNum) lineLen = 0;
        //if(++lineSended == 1000) {
        //    lineSended = 0;
        //    emit sendFileSize(fileSize);
        //    fileSize = 0;
        //}
        //emit sendFileSize(line.size());
        //emit sendFileSize(MssLen * 128 - 5);
        return proDatagram;
    }

    return nullptr;
}

void Sender::getFileName(const QString &fileName)
{
    file.setFileName(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return;
    qDebug() << file.size();
    emit sendFileSize(0);
    emit sendFileMaxSize(file.size());
}

void Sender::getIpAddress(QHostAddress ipAddr, quint16 port)
{
    addr = ipAddr;
    this->port = port;
    lineLen = 0;
    lineLen = 0;
    lineSended = 0;
    threadIndex = 0;
    head = 0;
    tail = head + windowSize -1;
    ackedRec.clear();
    sendFile();
}

void Sender::readPendingDatagrams()
{
    qDebug() << "reading";
    while(udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QBuffer buffer(&datagram);
        buffer.open(QIODevice::ReadOnly);
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size(),&addr,&port);

        ProtocalDatagram proDatagram;
        QDataStream in(&buffer);
        in >> proDatagram;
        buffer.close();
        qDebug() << "receive ack" << proDatagram.index << proDatagram.isAck;

        if(proDatagram.isAck==1) {
            if ((head < tail && proDatagram.index >=head && proDatagram.index <= tail)
               || (head > tail && (proDatagram.index >= head || proDatagram.index <= tail))) {
                while (head != proDatagram.index ) {
                    ProtocalDatagram* m_pDatagram = readFile();
                    if(m_pDatagram == nullptr) {
                        if (++head == groupNum) head = 0;
                        timer->stop();
                        if (head != tail)
                            timer->start(1000);
                    }
                    if (m_pDatagram != nullptr) {
                        /************发送数据************/

                        //sendToThread(*m_pDatagram);
                        /************发送数据************/
                        //窗口向前滑动

                        ackedRec[head]->first = false;
                        delete ackedRec[head]->second;
                        ackedRec[head]->second = NULL;
                        multiAck[head] = 0;
                        if (++head == groupNum) head = 0;
                        if (++tail == groupNum) tail = 0;
                        sendData(*m_pDatagram);
                        timer->stop();
                        timer->start(1000);
                        ackedRec[tail]->first=false;
                        ackedRec[tail]->second = m_pDatagram;
                        multiAck[head] = 0;
                        qDebug() << "after"  << ackedRec.size() << head << tail;
                    }
                }
                qDebug() << "fit";

            }
            qDebug() << "send ok!";
        }
    }

}

void Sender::timeoutResend()
{
    qDebug() << "------------timeResend-----------";
    if (udpSocket->bytesAvailable()!=0) {
       qDebug() << "socket failes, restart!";
        udpSocket->close();
        initSocket();
    }
    qDebug() << "------------resending------------";
    //sendData(*(ackedRec.at(head)->second));
        for (int i = head ; i != head + windowSize - 1 ; i++) {
            int index = i % groupNum;

            if (ackedRec.at(index)->first == false){
                qDebug() << NotReceiveNum << ackedRec[index]->second->index << i;

                sendData(*(ackedRec.at(index)->second));
            }
        }

    //sendData(*(ackedRec[head]->second));
    qDebug() << "------------timeout-------------";
}

void Sender::sendToThread(const ProtocalDatagram &prodata)
{
    //qDebug() << "threadIndex" << threadIndex << ":" << threadpool.at(threadIndex);
    //threadpool[threadIndex]->sendData(prodata);
    //if(++threadIndex == 4) threadIndex = 0;
}
