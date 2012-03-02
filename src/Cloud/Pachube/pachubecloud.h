#ifndef PACHUBE_CLOUD_H
#define PACHUBE_CLOUD_H

#include "Cloud/cloconnection.h"
#include "pachubexml.h"
#include <QHttp>
#include <QMap>
#include "Message/message.h"

class Configurator;

//TODO: timeout
//TODO: retry time
//TODO: ordersDone check timestamp of messages
//TODO: set ordersFeed
class PachubeCloud: public CloConnection{
    Q_OBJECT
    
public:
    PachubeCloud(Configurator*);
    PachubeCloud(QString feed, QString sendfeed, QString key);
    virtual CloConnection* create(Configurator*);
    virtual CloConnection* clone(Configurator*);
    virtual void connect();
    virtual ~PachubeCloud();
    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();
    virtual bool isBusy();
    void getOrders();

private:
    void send();
    void removeOldOrders(QVector<Message> &);
    void updateOrders(const QVector<Message> &);

    PachubeXml currentPachubeXml;
    QString sendFeed;
    QString ordersFeed;
    QString apiKey;
    bool busy;
    QHttp http;
    QHttp orderHttp;
    QMap <QString, Message> last_messages;
    QDateTime last_time;

public slots:
    void done(bool);
    void ordersDone(bool);
    void retry();
    void getOrdersSlot();
    void catchSslErrors ( const QList<QSslError> & errors );

signals:
    void orderReceived(QVector<Message>);

};
    
#endif
