#ifndef PACHUBE_CLOUD_H
#define PACHUBE_CLOUD_H

#include "src/Cloud/cloconnection.h"
#include "pachubexml.h"
#include <QHttp>
#include "src/Message/message.h"

class Configurator;

//TODO: timeout
//TODO: retry time
//TODO: ordersDone check timestamp of messages
class PachubeCloud: public CloConnection{
    Q_OBJECT
    
public:
    PachubeCloud(Configurator*);
    virtual CloConnection* create(Configurator*);
    virtual CloConnection* clone(Configurator*);
    virtual ~PachubeCloud();
    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();
    virtual bool isBusy();

private:
    void send();

    PachubeXml currentPachubeXml;
    QString sendFeed;
    QString ordersFeed;
    QString apiKey;
    bool busy;
    QHttp http;
    QHttp orderHttp;

public slots:
    void done(bool);
    void ordersDone(bool);
    void retry();
    void getOrders();

signals:
    void orderReceived(QVector<Message>);

};
    
#endif
