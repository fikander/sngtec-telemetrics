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

    // schedule this API call for deletion
    this->deleteLater();

    if (error)
    {
        QWARNING << "HTTP connection error (request " << requestId << "): " <<
                    context->baseUrl.toString() << http.error() << http.errorString();
    }
    else
    {
        QHttpResponseHeader response = http.lastResponse();
        if (response.statusCode() >= 400)
        {
            QWARNING << "HTTP error (request " << requestId << "):" <<
                        QVariant(response.statusCode()).toString() <<
                        context->baseUrl.toString() << getMethod() << getAPI() << getContent();
        }
        else
        {
            QDEBUG << "HTTP call SUCCESS (request " << requestId << "): " <<
                      QVariant(response.statusCode()).toString();
        }
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
    bool first = true;

    foreach(QSharedPointer<MessageSample> sample, samples)
    {
        Q_ASSERT(sample->isLocked());

        // {\"at\":\"2010-05-20T11:01:44Z\",\"value\":\"295\"},
        if (!first)
            content += ",";
        first = false;
        content += "{\"at\":\"" + sample->timestamp.toString(Qt::ISODate) + "\",";
        content += "\"value\":\"" + sample->value + "\"}";
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
    else
        // unlock so that Cloud picks them up and sends later
        foreach(QSharedPointer<MessageSample> sample, samples)
            sample->setLocked(false);
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
    Q_ASSERT(event->isLocked());

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
    else
        event->setLocked(false);
}


APICallGetDataStreams::APICallGetDataStreams(
        QSharedPointer<SNGConnectAPI> context,
        QString filter,
        QQueue< QSharedPointer<Message> > *receivedMessages) :
    APICall(context), filter(filter), receivedMessages(receivedMessages)
{
}

void APICallGetDataStreams::parseJSONResponse(QString response)
{

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + response + ")"); // In new versions it may need to look like engine.evaluate("(" + QString(result) + ")");

    if (sc.property("datastreams").isArray())
    {
           QStringList items;
           qScriptValueToSequence(sc.property("datastreams"), items);

           foreach (QString str, items) {
               QDEBUG << str;
            }

    }
}

void APICallGetDataStreams::done(bool error)
{
    APICall::done(error);

    if (!error /*&& http.lastResponse().statusCode() == 200*/)
    {
        QString test="\
        {\
         \"datastreams\" : [ {\
             \"value_requested_at\" : \"2010-06-25T11:54:17.454020Z\",\
             \"requested_value\" : \"999\",\
             \"label\" : \"param1\"\
          },\
          {\
             \"value_requested_at\" : \"2010-06-24T10:05:49.000000Z\",\
             \"requested_value\" : \"0000017\",\
             \"label\" : \"param2\"\
          } ]\
        }\
        ";
        parseJSONResponse(test);

        parseJSONResponse(http.readAll());

        receivedMessages->append(QSharedPointer<Message>(new MessageRequest()));
    }
}
