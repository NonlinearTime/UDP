#include "widget.h"
#include "ui_widget.h"
#include <QThread>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    fileReceived = 0;
    fileSizePerSec = 0;
    time = 0;
    receiver = new Receicer();
    connect(receiver,SIGNAL(sendedSize(qint64)),this,SLOT(upDateSpeed(qint64)));
    QThread *thread = new QThread();
    receiver->moveToThread(thread);
    thread->start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    receiver->addr = QHostAddress(ui->IplineEdit->text());
    receiver->port = ui->PortlineEdit->text().toInt();
    receiver->initContainer();
    time = 0;
    displaySpeed();
}

void Widget::on_testButton_clicked()
{
    QUdpSocket socket;
    socket.writeDatagram("1",1,QHostAddress(ui->IplineEdit->text()),ui->PortlineEdit->text().toInt());
    receiver->initContainer();
    //receiver->file.
}

void Widget::upDateSpeed(qint64 receivedSize)
{
    fileReceived += receivedSize;
    fileSizePerSec += receivedSize;
}

void Widget::displaySpeed()
{
    fileSizePerSec = ReceivedFile::GetInstance()->fileSize - fileReceived;
    fileReceived = ReceivedFile::GetInstance()->fileSize;
    float speed = fileSizePerSec / 1000 ;
    ui->SpeedLaebl->setText(QString("size: %1M  speed: %2M/s").arg(fileReceived/1000 / 1000).arg(speed/1000));
    ui->timeLabel->setText(QString("%1 s").arg(time++));
    QTimer::singleShot(1000,this,SLOT(displaySpeed()));
}
