#ifndef SNGCONNECTAPI_H
#define SNGCONNECTAPI_H

#include <QObject>
#include <QSharedPointer>
#include <QHttp>
#include <QUrl>

#include <debug.h>
#include "KeyValueMap.h"
#include "Message/message.h"

class SNGConnectAPI : public QObject
{
    Q_OBJECT
public:
    SNGConnectAPI(KeyValueMap &config);
    ~SNGConnectAPI();

protected:
    bool makeHttpRequest(QString method, QString api, QString contents);

    QUrl baseUrl;
    QString feed;
    QHttp http;

friend class APICall;
};

/*
 *
 */
class APICall : public QObject
{
    Q_OBJECT

public:
    static QList<APICall*> doneAPICalls;
    static void cleanup();

    APICall(QSharedPointer<SNGConnectAPI> context);

    virtual ~APICall();

    void invoke();

protected slots:

    virtual void done(bool error);

protected:
    virtual QString getMethod() = 0;
    virtual QString getAPI() = 0;
    virtual QString getContent() = 0;

    QHttp &http() { return context->http; }
    QString &feed() { return context->feed; }

    QSharedPointer<SNGConnectAPI> context;
};

//void sendEvents(QList< QSharedPointer<Message> > &events);
//void sendDatastreamSamples(QString datastream, QList< QSharedPointer<MessageSample> > &samples);

/*
 *
 *
 *   PUT api/v1/feeds/{id}/datastreams/{id}.json
 *   {
 *     "datapoints":[
 *       {"at":"2010-05-20T11:01:43Z","value":"294"},
 *       {"at":"2010-05-20T11:01:44Z","value":"295"},
 *       {"at":"2010-05-20T11:01:45Z","value":"296"},
 *       {"at":"2010-05-20T11:01:46Z","value":"297"}
 *     ]
 *   }
 */
class APICallSendDatastreamSamples : public APICall
{
    Q_OBJECT
public:
    APICallSendDatastreamSamples(
            QSharedPointer<SNGConnectAPI> context,
            QString datastream,
            QList< QSharedPointer<MessageSample> > &samples);

protected slots:
    virtual void done(bool error);

protected:
    virtual QString getMethod() { return "PUT"; }
    virtual QString getAPI() { return "api/v1/feeds/" + feed() + "/datastreams/" + datastream + ".json"; }
    virtual QString getContent() { return content; }

    QString datastream;
    QString content;
};


/*
 *
 *       POST api/v1/feeds/{id}/events.json
 *       {
 *           "type": "alarm_on|alarm_off|information|system_error|system_warning",
 *           "id": "1",
 *           "timestamp": "<timestamp>"
 *           "message": "somethings gone wrong"
 *       }
 *
 */
class APICallSendEvent : public APICall
{
    Q_OBJECT
public:
    APICallSendEvent(
            QSharedPointer<SNGConnectAPI> context,
            QSharedPointer<Message> &event);

protected slots:
    virtual void done(bool error);

protected:
    virtual QString getMethod() { return "POST"; }
    virtual QString getAPI() { return "api/v1/feeds/" + feed() + "/events.json"; }
    virtual QString getContent() { return content; }

    QString content;
};

#endif // SNGCONNECTAPI_H
