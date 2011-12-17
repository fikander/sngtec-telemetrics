#include "pachube.h"

Pachube::Pachube(const QString &apikey, const QString &feed, const QDomDocument &xml):feed(feed), xml(xml){
                
    connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));

    QHttpRequestHeader header("PUT", feed);
    header.setValue("Host", "api.pachube.com");
    header.setValue("X-PachubeApiKey", apikey);
    header.setContentType("application/xml");

    http.setHost("api.pachube.com", QHttp::ConnectionModeHttps);
    http.request(header, xml.toString().toUtf8());
    
}
    
void Pachube::done(bool error){
    if (error) {
        qDebug() << "error: " <<  http.errorString();
        emit aborted();
    }
    else {
        qDebug("data sended");
        emit send();
    }
}
