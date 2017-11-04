#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "sender.h"
#include <QtNetwork>
#include <sendedfile.h>
#include <QTimer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    quint64 filereceived;
    quint64 filePerSec;

private:
    Ui::Widget *ui;
    Sender *udpsender;

signals:
    void setFileName(const QString &fileName);
    void setUdpAddress(QHostAddress Ip,quint16 Port);

private slots:

    void on_fileButton_clicked();
    void on_portButton_clicked();
    void on_MssButton_clicked();
    void updateProgressBar();
    void setProgressBarMax(quint64 size);
};

#endif // WIDGET_H
