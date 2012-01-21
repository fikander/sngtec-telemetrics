#include "pachubecloud.h"
#include <QDebug>
#include <QVector>
#include <QTimer>
#include "Message/message.h"
#include "Configurator/configurator.h"


PachubeCloud::PachubeCloud(Configurator* config)
    : currentPachubeXml(config->getFeed()), sendFeed(config->getFeed()), apiKey(config->getApiKey()) {
    busy = false;
    ordersFeed = sendFeed;
}
     

CloConnection* PachubeCloud::create(Configurator* config) {
    return new PachubeCloud(config);
}

CloConnection* PachubeCloud::clone(Configurator* config) {
    return new PachubeCloud(config);
}

PachubeCloud::~PachubeCloud() {
}

void PachubeCloud::write(QVector<Message> messages) {
    PachubeXml pxml(sendFeed);
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
        qDebug() << "PachubeCloud send error: " <<  http.errorString();
        QTimer::singleShot(100000, this, SLOT(retry()));
    }
    else {
        busy = false;
        //qDebug() << "pachube cloud done" << http.readAll();
        emit readyToWrite();
    }
}
void PachubeCloud::retry(){
    send();
}

void PachubeCloud::send() {
    qDebug() << "sending";
    busy = true;

    connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));

    QHttpRequestHeader header("PUT", "/v2/feeds/" + sendFeed + ".xml");
    header.setValue("Host", "api.pachube.com");
    header.setValue("X-PachubeApiKey", apiKey);
    header.setContentType("application/xml");

    http.setHost("api.pachube.com", QHttp::ConnectionModeHttps);
    http.request(header, currentPachubeXml.getXml().toString().toUtf8());
}


void PachubeCloud::getOrders() {
    connect(&orderHttp, SIGNAL(done(bool)), this, SLOT(ordersDone(bool)));
    QHttpRequestHeader header("GET", "/v2/feeds/" + ordersFeed + ".xml");
    header.setValue("Host", "api.pachube.com");
    header.setValue("X-PachubeApiKey", apiKey);

    orderHttp.setHost("api.pachube.com", QHttp::ConnectionModeHttps);
    orderHttp.request(header);
}

void PachubeCloud::ordersDone(bool error) {
    if(error) {
        qDebug() << "PachubeCloud oreders receive error: " <<  orderHttp.errorString();
    }
    else {
        PachubeXml ordersXml = PachubeXml::PachubeFromXml(orderHttp.readAll());
        QVector<Message> messages = ordersXml.getMessages();
        emit orderReceived(messages);
    }
}

void PachubeCloud::getOrdersSlot() {
    getOrders();
}
