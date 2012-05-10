#include "cosmcloud.h"
#include <QDebug>
#include <QVector>
#include <QTimer>
#include <QNetworkRequest>
#include <QSslError>
#include "Message/message.h"
#include "Configurator/configurator.h"


CosmCloud::CosmCloud(Configurator* config)
    : currentCosmXml(config->getFeed()), sendFeed(config->getFeed()), apiKey(config->getApiKey()),
      last_time(QDateTime::currentDateTime()){
    busy = false;
    ordersFeed = sendFeed;
}

CosmCloud::CosmCloud(QString feed, QString sendfeed, QString key)
    : currentCosmXml(feed), sendFeed(feed), apiKey(key), last_time(QDateTime::currentDateTime()){
    busy = false;
    ordersFeed = sendFeed;
}
     
CloConnection* CosmCloud::create(Configurator* config) {
    return new CosmCloud(config);
}

CloConnection* CosmCloud::clone(Configurator* config) {
    return new CosmCloud(config);
}

CosmCloud::~CosmCloud() {
}

void CosmCloud::connect() {}

void CosmCloud::write(QVector<Message> messages) {
    CosmXml pxml(sendFeed);
    for(MessagesSet::const_iterator it = messages.begin(); it != messages.end(); ++it) {
        pxml.addData(*it);
    }
    currentCosmXml = pxml;
    send();
}

QVector<Message> CosmCloud::readAll() {
    return QVector<Message>();
}

bool CosmCloud::isBusy() {
    return busy;
}

void CosmCloud::done(bool error) {
    if (error) {
        qDebug() << "CosmCloud send error: " <<  http.errorString();
        QTimer::singleShot(100000, this, SLOT(retry()));
    }
    else {
        busy = false;
        qDebug() << "cosm cloud done" << http.readAll();
        emit readyToWrite();
    }
}
void CosmCloud::retry(){
    send();
}

void CosmCloud::send() {
    qDebug() << "sending";
    busy = true;

    //connect(&http, SIGNAL(sslEBBrrors),  this, SLOT(catchSslErrors));
    QObject::connect(&http, SIGNAL(sslErrors(const QList<QSslError> &)),  &http, SLOT(ignoreSslErrors()));
    QObject::connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));

    QHttpRequestHeader header("PUT", "/v2/feeds/" + sendFeed + ".xml");
    header.setValue("Host", "api.cosm.com");
    header.setValue("X-CosmApiKey", apiKey);
    header.setContentType("application/xml");

    http.setHost("api.cosm.com", QHttp::ConnectionModeHttps);
    //http.setHost("api.cosm.com");
    http.request(header, currentCosmXml.getXml().toString().toUtf8());
}


void CosmCloud::getOrders() {
    QObject::connect(&orderHttp, SIGNAL(done(bool)), this, SLOT(ordersDone(bool)));
    QHttpRequestHeader header("GET", "/v2/feeds/" + ordersFeed + ".xml");
    header.setValue("Host", "api.cosm.com");
    header.setValue("X-ApiKey", apiKey);

    orderHttp.setHost("api.cosm.com", QHttp::ConnectionModeHttps);
    orderHttp.request(header);
}

void CosmCloud::ordersDone(bool error) {
    if(error) {
        qDebug() << "CosmCloud orders receive error: " <<  orderHttp.errorString();
    }
    else {
        CosmXml ordersXml = CosmXml::CosmFromXml(orderHttp.readAll());
        QVector<Message> messages = ordersXml.getMessages();
        removeOldOrders(messages);
        updateOrders(messages);
        emit orderReceived(messages);
    }
}

void CosmCloud::getOrdersSlot() {
    getOrders();
}

void CosmCloud::catchSslErrors ( const QList<QSslError> & errors ) {
    qDebug() << "CosmCloud ssl errors: ";
    for(QList<QSslError>::const_iterator it = errors.begin();
            it != errors.end(); ++it) {
        qDebug() << it->errorString();
    }
}

void CosmCloud::removeOldOrders(QVector<Message> &messages) {
    for(QVector<Message>::iterator it = messages.begin();
        it != messages.end(); ++it) {
        if((last_messages.contains(it->key) &&
           last_messages.find(it->key)->timestamp == it->timestamp) ||
           it->timestamp < last_time) {
            messages.erase(it);
        }
    }
}

void CosmCloud::updateOrders(const QVector<Message> &messages) {
    last_messages.clear();
    for(QVector<Message>::const_iterator it = messages.begin();
        it != messages.end(); ++it) {
        last_messages.insert(it->key, *it);
    }
}
