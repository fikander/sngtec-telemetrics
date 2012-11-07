#ifndef COSM_CLOUD_H
#define COSM_CLOUD_H

#include "Cloud/cloconnection.h"
#include "cosmxml.h"
#include <QHttp>
#include <QMap>
#include <QTimer>
#include "Message/message.h"

class Configurator;

//TODO: timeout
//TODO: retry time
//TODO: ordersDone check timestamp of messages
//TODO: set ordersFeed
class CosmCloud: public CloConnection{
    Q_OBJECT
    
public:
    CosmCloud(Configurator*);
    CosmCloud(QString feed, QString sendfeed, QString key);
    virtual CloConnection* create(Configurator*);
    virtual CloConnection* clone(Configurator*);
    virtual void connect();
    virtual ~CosmCloud();
    virtual void write(QVector<MessageSample>);
    virtual QVector<MessageSample> readAll();
    virtual bool isBusy();

private:
    void send();
    void removeOldOrders(QVector<MessageSample> &);
    void updateOrders(const QVector<MessageSample> &);

    CosmXml currentCosmXml;
    QString sendFeed;
    QString ordersFeed;
    QString apiKey;
    bool busy;
    QHttp http;
    QHttp orderHttp;
    QMap <QString, MessageSample> last_messages;
    QDateTime last_time;
    QTimer requestTimer;

public slots:
    void getOrders();
    void done(bool);
    void ordersDone(bool);
    void retry();
    void getOrdersSlot();
    void catchSslErrors ( const QList<QSslError> & errors );

};
    
#endif
