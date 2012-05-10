#include "httplogsender.h"
#include "Message/message.h"
#include <QUrl>
#include <QDebug>

void HTTPLogSender::sendLogs(QString address, QFile file) {

    QUrl url(address);
    QHttpRequestHeader header("PUT", address);
    header.setValue("Host", url.host());

    QObject::connect(&http, SIGNAL(sslErrors(const QList<QSslError> &)),  &http, SLOT(ignoreSslErrors()));
    QObject::connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));

    http.setHost("api.cosm.com", QHttp::ConnectionModeHttps);
    http.request(header, &file);

}

void HTTPLogSender::done(bool error) {
    Message message;
    
    if(error) {
        qWarning() << "Log sending error" + http.errorString();
        message.value = http.errorString();
    }
    else {
        message.value = "send OK";
    }

    emit statusUpdate(message);

}

void HTTPLogSender::catchSslErrors ( const QList<QSslError> & errors ) {
    
}
