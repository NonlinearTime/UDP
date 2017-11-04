#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "receicer.h"
#include <QString>
#include <QTimer>
#include <receivedfile.h>

namespace Ui {
class Widget;
}


class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void displaySpeed();
private slots:
    void on_pushButton_clicked();

    void on_testButton_clicked();

    void upDateSpeed(qint64 receivedSize);

private:
    Ui::Widget *ui;
    Receicer *receiver;
    double fileReceived;
    quint64 time;
    double fileSizePerSec;
};

#endif // WIDGET_H
