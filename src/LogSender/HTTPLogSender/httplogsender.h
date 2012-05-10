#ifndef HTTPLOGSENDER_H
#define HTTPLOGSENDER_H

#include "LogSender/logsender.h"

#include <QFile>
#include <QHttp>

class HTTPLogSender : LogSender {
    Q_OBJECT

private:
    QHttp http;

public:

    virtual void sendLogs(QString, QFile);

public slots:
    void done(bool);
    void catchSslErrors ( const QList<QSslError> & errors );

};

#endif
