#ifndef SNGCONNECTAPI_H
#define SNGCONNECTAPI_H

#include <QObject>
#include <QSharedPointer>
#include <QHttp>
#include <QUrl>
#include <QQueue>

#include <debug.h>
#include "KeyValueMap.h"
#include "Message/message.h"

/*
 * API context
 */
class SNGConnectAPI : public QObject
{
    Q_OBJECT
public:
    SNGConnectAPI(KeyValueMap &config);
    ~SNGConnectAPI();

protected:
    QUrl baseUrl;
    QString feed;

friend class APICall;
};


/*
 * API call base class
 */
class APICall : public QObject
{
    Q_OBJECT

public:
    APICall(QSharedPointer<SNGConnectAPI> context);
    virtual ~APICall();

    void invoke();

protected slots:

    virtual void done(bool error);

protected:

    bool makeHttpRequest(QString method, QString api, QString contents);

    virtual QString getMethod() = 0;
    virtual QString getAPI() = 0;
    virtual QString getContent() = 0;

    QString &feed() { return context->feed; }

    QHttp http;
    // each http.request returns id to track it
    int requestId;

    QSharedPointer<SNGConnectAPI> context;
};

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
    virtual QString getContent();

    QString datastream;
    QList< QSharedPointer<MessageSample> > samples;
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
    virtual QString getContent();

    QSharedPointer<MessageEvent> event;
};

/*
  GET api/v1/feeds/{id}/datastreams.json?filter=requested
{
 "datastreams" : [ {
     "value_requested_at" : "2010-06-25T11:54:17.454020Z",
     "requested_value" : "999",
     "label" : "param1"
  },
  {
     "value_requested_at" : "2010-06-24T10:05:49.000000Z",
     "requested_value" : "0000017",
     "label" : "param2"
  } ]
}
*/

class APICallGetDataStreams : public APICall
{
    Q_OBJECT
public:
    APICallGetDataStreams(
            QSharedPointer<SNGConnectAPI> context,
            QSharedPointer<Message> semaphore,
            QString filter,
            QQueue< QSharedPointer<Message> > *receivedMessages);

protected slots:
    virtual void done(bool error);

protected:
    virtual QString getMethod() { return "GET"; }
    virtual QString getAPI() { return "api/v1/feeds/" + feed() + "/datastreams.json?filter=" + filter; }
    virtual QString getContent() { return ""; }

    QSharedPointer<Message> semaphore;
    QString filter;
    QQueue< QSharedPointer<Message> > *receivedMessages;

    void parseJSONResponse(QString response, QQueue<QSharedPointer<Message> > &result);
};

/*
  GET commands
{
"commands" : [

    {"command": "reboot"},

    {"command": "upload_log",
     "arguments": {
         "url": "http://sngconnect/logs/ad324DZU",
         "start": "<timestamp>" }
    }]
}
*/

class APICallGetCommands : public APICall
{
    Q_OBJECT
public:
    APICallGetCommands(
            QSharedPointer<SNGConnectAPI> context,
            QSharedPointer<Message> semaphore,
            QQueue< QSharedPointer<Message> > *receivedMessages);

protected slots:
    virtual void done(bool error);

protected:
    virtual QString getMethod() { return "GET"; }
    virtual QString getAPI() { return "api/v1/feeds/" + feed() + "/commands.json"; }
    virtual QString getContent() { return ""; }

    QSharedPointer<Message> semaphore;
    QQueue< QSharedPointer<Message> > *receivedMessages;

    void parseJSONResponse(QString response, QQueue<QSharedPointer<Message> > &result);
};

/*
 *
 */
class APICallSendLog : public APICall
{
    Q_OBJECT
public:
    APICallSendLog(
            QSharedPointer<SNGConnectAPI> context,
            QSharedPointer<MessageResponse> &response);

protected slots:
    virtual void done(bool error);

protected:
    virtual QString getMethod() { return "POST"; }
    virtual QString getAPI() { return "api/v1/upload-log/" + log_request_id + "/" + log_request_hash + ".json"; }
    virtual QString getContent();

    QSharedPointer<MessageResponse> response;
    QString log_request_id, log_request_hash;
};

#endif // SNGCONNECTAPI_H
