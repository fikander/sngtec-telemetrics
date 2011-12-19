#ifndef DEVPROXY_H
#define DEVPROXY_H

#include <QDebug>
#include <QObject>
#include <QAbstractSocket>
#include "src/Message/message.h"
#include "src/Configurator/configurator.h"

class CloProxy;
class DevConnection;
class Message;
class Configurator;

class DevProxy : public QObject {
    Q_OBJECT


private:
    CloProxy *clo;

signals:
    void enque(Message msg);

public:
    DevConnection *ioDevice;

    DevProxy();
    DevProxy(Configurator*);
    //    ~DevProxy();

public slots:
    void readDevice();   // Podpiete do readyRead z socketa
    void readCloud();    // Podpiete do ?
    void displayError(QAbstractSocket::SocketError);
    void connectCloud(CloProxy *cl);


};

#endif // DEVPROXY_H
