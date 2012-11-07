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
#include "LogSender/logsender.h"

class CloConnection;
class DevProxy;
class Configurator;
class MessageSample;

class CloProxy : public QObject {
  Q_OBJECT

private:
    int askInterval;
    Configurator *configurator;
    DevProxy *dev;
    CloConnection *ioDevice;
    QQueue<MessageSample> que;
    void dispatchMessage(MessageSample m);
    QVector<DevProxy*> devList;
    LogSender *sender;

public:
    CloProxy(Configurator *config);
    void connectDev(DevProxy *dv);
    QTimer *timer;

public slots:
    void askServer();
    void queue(MessageSample payload);
    void senderResponse(MessageSample payload);
    void receiveServerMessages(QVector<MessageSample> );
};

#endif // CLOPROXY_H
