#ifndef LOGSENDER_H
#define LOGSENDER_H

#include <QObject>
#include <QFile>
#include "Message/message.h"


class LogSender : public QObject {
    Q_OBJECT

public:
    virtual void sendLogs(QString, QVector<QString>) = 0;

signals:
    void statusUpdate (Message);
};

#endif
