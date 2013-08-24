#include "httplogsender.h"
#include "Message/message.h"
#include <QUrl>
#include <QDebug>

HTTPLogSender::HTTPLogSender() {
    QObject::connect(&http, SIGNAL(sslErrors(const QList<QSslError> &)),
                     &http, SLOT(ignoreSslErrors()));
    QObject::connect(&http, SIGNAL(done(bool)),
                     this, SLOT(done(bool)));
}

void HTTPLogSender::sendLogs(QString address, QVector<QString> log) {
    int size = 0;
    QString dataFile;
    for(QVector<QString>::const_iterator it = log.begin(); it < log.end(); ++it) {
        size = it->size();
    }
    dataFile.reserve(size);
    for(QVector<QString>::const_iterator it = log.begin(); it < log.end(); ++it) {
        dataFile += *it;
    }

    QUrl url(address);
    QHttpRequestHeader header("PUT", address);
    header.setValue("Host", url.host());

    http.setHost(url.host(), QHttp::ConnectionModeHttps);
    http.request(header, dataFile.toAscii());
}

void HTTPLogSender::sendLogs(QString address, QFile file) {

    QUrl url(address);
    QHttpRequestHeader header("PUT", address);
    header.setValue("Host", url.host());


    http.setHost(url.host(), QHttp::ConnectionModeHttps);
    http.request(header, &file);
}

void HTTPLogSender::done(bool error) {
    QString message;
    
    if(error) {
        qWarning() << "Log sending error" + http.errorString();
        message = http.errorString();
    }
    else {
        message = "send OK";
    }

    emit statusUpdate(message);
}

void HTTPLogSender::catchSslErrors ( const QList<QSslError> & errors ) {
    
}
