#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->spinBox->setMaximum(1000);
    ui->spinBox->setMinimum(0);

    filereceived = 0;
    filePerSec = 0;

    udpsender = new Sender(this);
    connect(this,SIGNAL(setFileName(const QString&)),udpsender,SLOT(getFileName(const QString&)));
    connect(this,SIGNAL(setUdpAddress(QHostAddress,quint16)),udpsender,SLOT(getIpAddress(QHostAddress,quint16)));
    //connect(udpsender,SIGNAL(sendFileSize(quint64)),this,SLOT(updateProgressBar(quint64)));
    connect(udpsender,SIGNAL(sendFileMaxSize(quint64)),this,SLOT(setProgressBarMax(quint64)));
}

Widget::~Widget()
{
    delete ui;
}



void Widget::on_fileButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("选择文件"));
    emit setFileName(filename);
    updateProgressBar();
}

void Widget::on_portButton_clicked()
{
    QHostAddress ip(ui->ipLineEdit->text());
    quint16 port = ui->portLineEdit->text().toInt();
    emit setUdpAddress(ip,port);
}

void Widget::on_MssButton_clicked()
{
    udpsender->MssLen = ui->spinBox->value();
}

void Widget::updateProgressBar()
{
    filePerSec = SendedFile::GetInstance()->fileSize - filereceived;
    filereceived = SendedFile::GetInstance()->fileSize;
    ui->progressBar->setValue(filereceived);
    QTimer::singleShot(100,this,SLOT(updateProgressBar()));
}

void Widget::setProgressBarMax(quint64 size)
{
    ui->progressBar->setMaximum(size);
}
