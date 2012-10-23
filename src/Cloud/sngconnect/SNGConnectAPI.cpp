#include "debug.h"

#include <QNetworkRequest>
//http://qtwiki.org/Parsing_JSON_with_QT_using_standard_QT_library
//http://qt-project.org/doc/note_revisions/69/106/view
#include <QtScript/QScriptEngine>

#include "SNGConnectAPI.h"


SNGConnectAPI::SNGConnectAPI(KeyValueMap &config)
{
    if (config.contains("baseurl"))
        baseUrl.setUrl(config["baseurl"].toString(), QUrl::StrictMode);
    else
        baseUrl.setUrl("http://localhost");

    Q_ASSERT(baseUrl.isValid());

    feed = config["feed"].toString();
}

SNGConnectAPI::~SNGConnectAPI()
{
}


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

APICall::APICall(QSharedPointer<SNGConnectAPI> context):
    context(context)
{
    requestId = 0;
}

APICall::~APICall()
{
    QDEBUG << "Deleting APICall " << requestId;
    context.clear();
}

bool APICall::makeHttpRequest(QString method, QString api, QString contents)
{
    QDEBUG << "Request: " << context->baseUrl.scheme() << context->baseUrl.host() << context->baseUrl.port() << method << api << contents;

    QHttpRequestHeader header(method, QUrl::toPercentEncoding(api));

    header.setValue("Host", context->baseUrl.host());
    header.setContentType("application/json");

    http.setHost(context->baseUrl.host(),
        context->baseUrl.scheme() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp,
        context->baseUrl.port());
    requestId = http.request(header, contents.toUtf8());

    return true;
}


void APICall::invoke()
{
    QObject::connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));
    makeHttpRequest(getMethod(), getAPI(), getContent());
    QDEBUG << "Http request done: " << requestId;
}

void APICall::done(bool error)
{
    // disconnect everything from done(bool) signal
    QObject::disconnect(&http, SIGNAL(done(bool)), 0, 0);

    // schedule for deletion
    this->deleteLater();

    if (error)
    {
        QDEBUG << "done with ERROR " << requestId;
    }
    else
    {
        QHttpResponseHeader response = http.lastResponse();
        QDEBUG << "done with SUCCESS " << requestId << ": " << QVariant(response.statusCode()).toString();
        QDEBUG << http.readAll();
    }
}



APICallSendDatastreamSamples::APICallSendDatastreamSamples(
        QSharedPointer<SNGConnectAPI> context,
        QString datastream,
        QList< QSharedPointer<MessageSample> > &samples) :
    APICall(context), datastream(datastream), samples(samples)
{

}

QString APICallSendDatastreamSamples::getContent()
{
    QString content = "{\"datapoints\":[";

    foreach(QSharedPointer<MessageSample> sample, samples)
    {
        // {\"at\":\"2010-05-20T11:01:44Z\",\"value\":\"295\"},

        content += "{\"at\":\"" + sample->timestamp.toString(Qt::ISODate) + "\",";
        content += "\"value\":\"" + sample->value + "\"}";
        if (sample != samples.last())
            content += ",";
    }

    content += "]}";
    return content;
}

void APICallSendDatastreamSamples::done(bool error)
{
    APICall::done(error);

    if (!error && http.lastResponse().statusCode() == 200)
        foreach(QSharedPointer<MessageSample> sample, samples)
            sample->setProcessed();
}



APICallSendEvent::APICallSendEvent(
        QSharedPointer<SNGConnectAPI> context,
        QSharedPointer<Message> &event) :
    APICall(context)
{
    this->event = event.staticCast<MessageEvent>();
}

QString APICallSendEvent::getContent()
{
    /*       {
     *           "type": "alarm_on|alarm_off|information|system_error|system_warning",
     *           "id": "1",
     *           "timestamp": "<timestamp>"
     *           "message": "something’s gone wrong"
     *       }
     */
    QString content  = "{\"type\":\"information\",";
    content += "\"id\":\"1\",";
    content += "\"timestamp\":\"" + event->timestamp.toString(Qt::ISODate) + "\",";
    content += "\"message\":\"" + event->toString() + "\"}";
    return content;
}


void APICallSendEvent::done(bool error)
{
    APICall::done(error);

    if (!error && http.lastResponse().statusCode() == 200)
        event->setProcessed();
}
