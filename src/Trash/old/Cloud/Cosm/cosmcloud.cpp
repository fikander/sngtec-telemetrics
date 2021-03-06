#include "cosmcloud.h"
#include <QDebug>
#include <QVector>
#include <QTimer>
#include <QNetworkRequest>
#include <QSslError>
#include "Message/message.h"
#include "Configurator/configurator.h"


CosmCloud::CosmCloud(Configurator* config)
    : currentCosmXml(config->getSendFeed()), sendFeed(config->getSendFeed()), ordersFeed(config->getReceiveFeed()), apiKey(config->getApiKey()),
      last_time(QDateTime::currentDateTime()){
    QObject::connect(&http, SIGNAL(sslErrors(const QList<QSslError> &)),  &http, SLOT(ignoreSslErrors()));
    QObject::connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));
    QObject::connect(&orderHttp, SIGNAL(done(bool)), this, SLOT(ordersDone(bool)));
    QObject::connect(&requestTimer, SIGNAL(timeout()), this, SLOT(getOrdersSlot()));
    busy = false;
    last_time = last_time.addSecs(config->getCosmTimeDifference().toInt() * 60 * 60);
    requestTimer.start(config->getRequestTime().toInt() * 1000);
}

CosmCloud::CosmCloud(QString feed, QString sendfeed, QString key)
    : currentCosmXml(feed), sendFeed(feed), apiKey(key), last_time(QDateTime::currentDateTime()){
    QObject::connect(&http, SIGNAL(sslErrors(const QList<QSslError> &)),  &http, SLOT(ignoreSslErrors()));
    QObject::connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));
    QObject::connect(&orderHttp, SIGNAL(done(bool)), this, SLOT(ordersDone(bool)));
    busy = false;
    ordersFeed = sendFeed;
    last_time = last_time.addSecs(-2 * 60 * 60);
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

void CosmCloud::write(QVector<MessageSample> messages) {
    //qDebug() << __PRETTY_FUNCTION__ << "sending data";
    CosmXml pxml(sendFeed);
    for(MessagesSet::const_iterator it = messages.begin(); it != messages.end(); ++it) {
        pxml.addData(*it);
    }
    currentCosmXml = pxml;
    send();
}

QVector<MessageSample> CosmCloud::readAll() {
    return QVector<MessageSample>();
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
        //qDebug() << "cosm cloud done" << http.readAll();
        emit readyToWrite();
    }
}
void CosmCloud::retry(){
    send();
}

void CosmCloud::send() {
    busy = true;

    //connect(&http, SIGNAL(sslEBBrrors),  this, SLOT(catchSslErrors));

    QHttpRequestHeader header("PUT", "/v2/feeds/" + sendFeed + ".xml");
    header.setValue("Host", "api.cosm.com");
    header.setValue("X-ApiKey", apiKey);
    header.setContentType("application/xml");

    http.setHost("api.cosm.com", QHttp::ConnectionModeHttps);
    //http.setHost("api.cosm.com");
    http.request(header, currentCosmXml.getXml().toString().toUtf8());
}


void CosmCloud::getOrders() {
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
        //qDebug() << orderHttp.readAll();
        QVector<MessageSample> oldmessages = ordersXml.getMessages();
        QVector<MessageSample> messages;
        foreach (MessageSample m, oldmessages) {
            m.key.replace("_", "|");
            messages.push_back(m);
        }
        removeOldOrders(messages);
        updateOrders(messages);
        //qDebug() << "messages received: " << messages.size();
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

void CosmCloud::removeOldOrders(QVector<MessageSample> &messages) {
    //qDebug() << "Message count " << messages.size();
    for(QVector<MessageSample>::iterator it = messages.begin();
        it != messages.end();) {
        if((last_messages.contains(it->key) &&
           last_messages.find(it->key)->timestamp == it->timestamp) ||
           it->timestamp < last_time) {
            //qDebug() << "Deleting message: " << it->timestamp << ", last time: " << last_time << endl;
            messages.erase(it);
        }
        else {
            //qDebug() << "Not Deleting message: " << it->timestamp << ", last time: " << last_time << endl;
            ++it;
        }
    }
}

void CosmCloud::updateOrders(const QVector<MessageSample> &messages) {
    for(QVector<MessageSample>::const_iterator it = messages.begin();
        it != messages.end(); ++it) {
        last_messages.remove(it->key);
        last_messages.insert(it->key, *it);
    }
}
