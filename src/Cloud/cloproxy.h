#ifndef CLOPROXY_H
#define CLOPROXY_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QQueue>
#include <QVector>

#include "Message/message.h"
#include "Configurator/configurator.h"
#include "Logger/logger.h"

class CloConnection;
class DevProxy;
class Configurator;
class Message;

class CloProxy : public QObject {
  Q_OBJECT

private:
    int askInterval;
    Configurator *configurator;
    DevProxy *dev;
    CloConnection *ioDevice;
    QQueue<Message> que;
    void dispatchMessage(Message m);
    QVector<DevProxy*> devList;

public:
    CloProxy(Configurator *config);
    void connectDev(DevProxy *dv);
    QTimer *timer;

public slots:
    void askServer();
    void queue(Message payload);
};

#endif // CLOPROXY_H
