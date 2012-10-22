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
    APICall::cleanup();
}

bool SNGConnectAPI::makeHttpRequest(QString method, QString api, QString contents)
{
    QDEBUG << "Request: " << baseUrl.scheme() << baseUrl.host() << baseUrl.port() << method << api << contents;

    QHttpRequestHeader header(method, QUrl::toPercentEncoding(api));

    header.setValue("Host", baseUrl.host());
    header.setContentType("application/json");

    http.setHost(baseUrl.host(),
        baseUrl.scheme() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp,
        baseUrl.port());
    http.request(header, contents.toUtf8());

    return true;
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

QList<APICall*> APICall::doneAPICalls;

void APICall::cleanup()
{
    qDeleteAll(doneAPICalls);
    doneAPICalls.clear();
}

APICall::APICall(QSharedPointer<SNGConnectAPI> context):
    context(context)
{
    cleanup();
}

APICall::~APICall()
{
    QDEBUG << "Deleting APICall";
}

void APICall::invoke()
{
    QObject::connect(&context->http, SIGNAL(done(bool)), this, SLOT(done(bool)));
    context->makeHttpRequest(getMethod(), getAPI(), getContent());

    QDEBUG << "Http request done";
}

void APICall::done(bool error)
{
    // disconnect everything from done(bool) signal
    QObject::disconnect(&context->http, SIGNAL(done(bool)), 0, 0);
    context.clear();

    // schedule for deletion
    doneAPICalls.append(this);
}



APICallSendDatastreamSamples::APICallSendDatastreamSamples(
        QSharedPointer<SNGConnectAPI> context,
        QString datastream,
        QList< QSharedPointer<MessageSample> > &samples) :
    APICall(context), datastream(datastream)
{
    content = "{\"datapoints\":[";

    foreach(QSharedPointer<MessageSample> sample, samples)
    {
        // {\"at\":\"2010-05-20T11:01:44Z\",\"value\":\"295\"},

        content += "{\"at\":\"" + sample->timestamp.toString(Qt::ISODate) + "\",";
        content += "\"value\":\"" + sample->value + "\"}";
        if (sample != samples.last())
            content += ",";

    }

    content += "]}";
}

void APICallSendDatastreamSamples::done(bool error)
{
    QHttpResponseHeader response = http().lastResponse();
    QDEBUG << "done " << QVariant(response.statusCode()).toString();
    QDEBUG << http().readAll();

    APICall::done(error);
}


APICallSendEvent::APICallSendEvent(
        QSharedPointer<SNGConnectAPI> context,
        QSharedPointer<Message> &event) :
    APICall(context)
{
    /*       {
     *           "type": "alarm_on|alarm_off|information|system_error|system_warning",
     *           "id": "1",
     *           "timestamp": "<timestamp>"
     *           "message": "something’s gone wrong"
     *       }
     */

    QSharedPointer<MessageEvent> e = event.staticCast<MessageEvent>();

    content  = "{\"type\":\"information\",";
    content += "\"id\":\"1\",";
    content += "\"timestamp\":\"" + e->timestamp.toString(Qt::ISODate) + "\",";
    content += "\"message\":\"" + e->toString() + "\"}";
}


void APICallSendEvent::done(bool error)
{
    QHttpResponseHeader response = http().lastResponse();
    QDEBUG << "done " << QVariant(response.statusCode()).toString();
    QDEBUG << http().readAll();

    APICall::done(error);
}
