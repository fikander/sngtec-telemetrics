#ifndef CLOPROXY_H
#define CLOPROXY_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QQueue>
#include "src/Message/message.h"

class DevProxy;

class CloProxy : public QObject {
  Q_OBJECT

private:
    int askInterval;
    DevProxy *dev;
    QQueue<Message> que;

public:
    CloProxy();

    void connectDev(DevProxy *dv);

    QTimer *timer;

public slots:
void askServer();
void queue(Message payload);
};

#endif // CLOPROXY_H
