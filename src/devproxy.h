#ifndef DEVPROXY_H
#define DEVPROXY_H

#include <QDebug>
#include <QObject>
#include <QAbstractSocket>
#include "cloproxy.h"

class CloProxy;

class DevProxy : public QObject {
    Q_OBJECT


private:
    CloProxy *clo;

signals:
    void enque(QString str);

public:
    QAbstractSocket *ioDevice;

    DevProxy();
    DevProxy(CloProxy* cloud);
    //    ~DevProxy();

public slots:
    void readDevice();   // Podpiete do readyRead z socketa
    void readCloud();    // Podpiete do ?
    void displayError(QAbstractSocket::SocketError);
    void connectCloud(CloProxy *cl);


};

#endif // DEVPROXY_H
