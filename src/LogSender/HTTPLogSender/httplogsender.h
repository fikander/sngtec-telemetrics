#ifndef HTTPLOGSENDER_H
#define HTTPLOGSENDER_H

#include "LogSender/logsender.h"

#include <QFile>
#include <QHttp>

class HTTPLogSender : public LogSender {
    Q_OBJECT

private:
    QHttp http;

public:
    HTTPLogSender();

    virtual void sendLogs(QString, QFile);
    virtual void sendLogs(QString, QVector<QString>);

public slots:
    void done(bool);
    void catchSslErrors ( const QList<QSslError> & errors );

};

#endif
