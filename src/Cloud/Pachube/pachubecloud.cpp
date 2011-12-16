#include "pachubecloud.h"
#include <QDebug>
#include <QVector>
#include <QTimer>
#include "src/Message/message.h"



PachubeCloud::PachubeCloud(Configurator &)
    : currentPachubeXml("41613")
    { //: apiKey(config.getApiKey()), feed(config.getFeed())
    apiKey = "xEc6VTbLaHKgmXMB24UCCXROcXm_hr2FWCFarVhHGtg";
    feed = "41613";
}
     

CloConnection* PachubeCloud::create(Configurator &config) {
    return new PachubeCloud(config);
}

CloConnection* PachubeCloud::clone(Configurator &config) {
    return new PachubeCloud(config);
}

PachubeCloud::~PachubeCloud() {
}

void PachubeCloud::write(QVector<Message> messages) {
    PachubeXml pxml(feed);
    for(MessagesSet::const_iterator it = messages.begin(); it != messages.end(); ++it) {
        pxml.addData(*it); 
    }
    currentPachubeXml = pxml;
    send();
}

QVector<Message> PachubeCloud::readAll() {
    return QVector<Message>();
}

bool PachubeCloud::isBusy() {
    return busy;
}

void PachubeCloud::done(bool error) {
    if (error) {
        qDebug() << "error: " <<  http.errorString();
        QTimer::singleShot(100000, this, SLOT(retry()));
    }
    else {
        busy = false;
        emit readyToWrite();
    }
}
void PachubeCloud::retry(){
    send();
}

void PachubeCloud::send() {
    busy = true;

    connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));
    connect(&http, SIGNAL(done(bool)), &http, SLOT(ignoreSslErrors()));

    QHttpRequestHeader header("PUT", "/v2/feed/" + feed + ".xml");
    header.setValue("Host", "api.pachube.com");
    header.setValue("X-PachubeApiKey", apiKey);
    header.setContentType("application/xml");

    http.setHost("api.pachube.com", QHttp::ConnectionModeHttps);
    http.request(header, currentPachubeXml.getXml().toString().toUtf8());
}
