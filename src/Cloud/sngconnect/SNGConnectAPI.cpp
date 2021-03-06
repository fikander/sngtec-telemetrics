#include "debug.h"

#include <QNetworkRequest>
//http://qtwiki.org/Parsing_JSON_with_QT_using_standard_QT_library
//http://qt-project.org/doc/note_revisions/69/106/view
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

#include "SNGConnectAPI.h"

#include "sha2/sha2.h"


SNGConnectAPI::SNGConnectAPI(KeyValueMap &config) :
    bytesReceived(0), bytesSent(0)
{
    if (config.contains("baseurl"))
        baseUrl.setUrl(config["baseurl"].toString(), QUrl::StrictMode);
    else
        baseUrl.setUrl("http://localhost");

    Q_ASSERT(baseUrl.isValid());

    feed = config["feed"].toString();

    if (config.contains("api_key"))
        apiKey = config["api_key"].toByteArray();
}


SNGConnectAPI::~SNGConnectAPI() { }


APICall::APICall(QSharedPointer<SNGConnectAPI> &context):
    context(context), requestId(0), bytesReceived(0), bytesSent(0)
 { }


APICall::~APICall()
{
    context.clear();
}


QString APICall::hmacSha256(QByteArray baseString)
{
    // HMAC-SHA-1 block size, defined in SHA-1 standard
    int blockSize = 64;
    if (context->apiKey.length() > blockSize) {
        // if key is longer than block size (64),
        // reduce key length with SHA-1 compression

        SHA256_CTX      tctx;
        QByteArray tk;

        SHA256_Init(&tctx);
        SHA256_Update(&tctx, (const u_int8_t*)context->apiKey.data(), context->apiKey.length());
        tk.resize(SHA256_DIGEST_LENGTH);
        SHA256_Final((u_int8_t *)tk.data(), &tctx);

        context->apiKey = tk;
    }

    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "\"
    // ascii characters 0x36 ("6") and 0x5c ("\") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

    for (int i = 0; i < context->apiKey.length(); i++) {
        // XOR operation between every byte in key and innerpadding, of key length
        innerPadding[i] = innerPadding[i] ^ context->apiKey.at(i);
        // XOR operation between every byte in key and outerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ context->apiKey.at(i);
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);

    // hash inner part
    SHA256_CTX context;
    QByteArray hashedPart;
    SHA256_Init(&context);
    SHA256_Update(&context, (const u_int8_t*)part.data(), part.length());
    hashedPart.resize(SHA256_DIGEST_LENGTH);
    SHA256_Final((u_int8_t *)hashedPart.data(), &context);

    total.append(hashedPart);

    // hash total
    QByteArray hashed;
    SHA256_Init(&context);
    SHA256_Update(&context, (const u_int8_t*)total.data(), total.length());
    hashed.resize(SHA256_DIGEST_LENGTH);
    SHA256_Final((u_int8_t *)hashed.data(), &context);

    // return hexdigest
    return hashed.toHex();
}


bool APICall::makeHttpRequest(QString method, QString api, QString contents)
{
    //TODO: only percent encode the part of the URL after first '?' 
    QHttpRequestHeader header(method, QUrl::toPercentEncoding(api, "/?&="));
    QByteArray contentsUtf8 = contents.toUtf8();
    bytesSent = contentsUtf8.size();

    header.setValue("Host", context->baseUrl.host());
    header.setContentType("application/json");
    header.setValue("Signature", hmacSha256(api.toAscii() + ":" + contentsUtf8));
    bytesSent += header.toString().size();

    http.setHost(context->baseUrl.host(),
        context->baseUrl.scheme() == "https" ?
            QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp,
        context->baseUrl.port());
    requestId = http.request(header, contentsUtf8);

    QDEBUG << "NEW HTTP request (id:" << requestId << "): " << context->baseUrl.scheme() <<
        context->baseUrl.host() << context->baseUrl.port() <<
        method << api << contents;

    return true;
}


void APICall::invoke()
{
    QObject::connect(&http, SIGNAL(done(bool)), this, SLOT(done(bool)));
    makeHttpRequest(getMethod(), getAPI(), getContent());
}


void APICall::done(bool error)
{
    // disconnect everything from done(bool) signal
    QObject::disconnect(&http, SIGNAL(done(bool)), 0, 0);

    // schedule this API call for deletion
    this->deleteLater();

    if (error) {
        QWARNING << "HTTP CONNECTION ERROR (id:" << requestId << "): " <<
            context->baseUrl.toString() << http.error() << http.errorString();
    } else {
        QHttpResponseHeader response = http.lastResponse();
        if (response.statusCode() >= 400) {
            QWARNING << "HTTP ERROR (id:" << requestId << "):" <<
                QVariant(response.statusCode()).toString() <<
                context->baseUrl.toString() << getMethod() <<
                getAPI() << "server said: " << http.readAll();
        } else {
            QDEBUG << "HTTP SUCCESS (id:" << requestId << "): " <<
                QVariant(response.statusCode()).toString();
        }
        bytesReceived = http.bytesAvailable();
    }

    this->processResponse(error);

    context->bytesSent += bytesSent;
    context->bytesReceived += bytesReceived;
}


APICallSendDatastreamSamples::APICallSendDatastreamSamples(
            QSharedPointer<SNGConnectAPI> &context,
            QList< MessageProxy* > &messages) :
    APICall(context), messages(messages) { }


QString APICallSendDatastreamSamples::getContent()
{
    bool firstDataStream = true;
    QString content = "{\"datastreams\":[";

    //sort datastreams into buckets by stream label
    QHash<QString, QList<MessageProxy*> > hash;

    foreach(MessageProxy* proxy, messages) {
        QSharedPointer<MessageSample> sample =
            proxy->object().staticCast<MessageSample>();
        hash[sample->key] += proxy;
    }

    foreach(QString key, hash.uniqueKeys()) {
        bool firstSample = true;
        if (!firstDataStream) {
            content += ",";
        } else {
            firstDataStream = false;
        }
        content += "{\"label\":\"" + key + "\",\"datapoints\":[";
        foreach(MessageProxy *proxy, hash[key]) {
            Q_ASSERT(proxy->isLocked());
            if (!firstSample) {
                content += ",";
            } else {
                firstSample = false;
            }
            QSharedPointer<MessageSample> sample =
                proxy->object().staticCast<MessageSample>();
            content += "{\"at\":\"" + sample->timestamp.toString(Qt::ISODate) + "\",";
            content += "\"value\":\"" + sample->value + "\"}";
        }
        content += "]}";
    }

    content += "]}";
    return content;
}

void APICallSendDatastreamSamples::processResponse(bool error)
{
    if (!error && http.lastResponse().statusCode() == 200) {
        foreach(MessageProxy *proxy, messages) {
            proxy->setProcessed();
        }
    } else {
        // unlock so that Cloud picks them up and sends later
        foreach(MessageProxy *proxy, messages) {
            proxy->setLocked(false);
            proxy->processingFailed();
        }
    }
}

APICallSendEvent::APICallSendEvent(
        QSharedPointer<SNGConnectAPI> &context,
        MessageProxy *proxy) :
    APICall(context), proxy(proxy) { }

QString APICallSendEvent::getContent()
{
    Q_ASSERT(proxy->isLocked());

    /*       {
     *           "type": "alarm_on|alarm_off|information|system_error|system_warning",
     *           "id": "1",
     *           "timestamp": "<timestamp>"
     *           "message": "something�s gone wrong"
     *       }
     */
    QSharedPointer<MessageEvent> event =
        proxy->object().staticCast<MessageEvent>();

    QString content  = "{\"type\":\"" + event->type + "\",";
    content += "\"id\":\"" + QString::number(event->id) + "\",";
    content += "\"timestamp\":\"" + event->timestamp.toString(Qt::ISODate) + "\",";
    content += "\"message\":\"" + event->toString() + "\"}";
    return content;
}


void APICallSendEvent::processResponse(bool error)
{
    if (!error && http.lastResponse().statusCode() == 200) {
        proxy->setProcessed();
    } else {
        proxy->setLocked(false);
        proxy->processingFailed();
    }
}


APICallGetDataStreams::APICallGetDataStreams(
        QSharedPointer<SNGConnectAPI> &context,
        QSemaphore *semaphore,
        QString filter,
        QQueue<QSharedPointer<Message> > *receivedMessages) :
    APICall(context), semaphore(semaphore), filter(filter),
    receivedMessages(receivedMessages) { }


/**
 * @brief APICallGetDataStreams::parseJSONResponse
 * Create MessageSample items in result. Can print some warnings if JSON is not understood.
 * @param response
 * @param result
 */
void APICallGetDataStreams::parseJSONResponse(
        QString response, QQueue< QSharedPointer<Message> > &result)
{
    QScriptValue sc, datastreams;
    QScriptEngine engine;
    sc = engine.evaluate("(" + response + ")");

    datastreams = sc.property("datastreams");

    if (datastreams.isArray())
    {
        QScriptValueIterator it(datastreams);

        while (it.hasNext()) {
            it.next();
            // skip non-dictionary items in the 'datastreams' array
            if (!it.value().isObject()) {
                continue;
            }

            QScriptValue value_requested_at =
                it.value().property("value_requested_at");
            QScriptValue requested_value =
                it.value().property("requested_value");
            QScriptValue label =
                it.value().property("label");

            if ( !( value_requested_at.isValid() &&
                    requested_value.isValid() &&
                    label.isValid()))
            {
                QWARNING << "Wrong format of response: " << response;
                return;
            }

            QVariant value(requested_value.toString());
            bool ok;
            float f_value = value.toFloat(&ok);
            if (!ok) {
                QWARNING << "Wrong format of value: " << value.toString();
                return;
            }

            QVariant timestamp(value_requested_at.toString());
            QDateTime d_timedate = timestamp.toDateTime();
            if (!d_timedate.isValid()) {
                QWARNING << "Wrong format of timestamp: " << timestamp.toString();
                return;
            }

            // TODO: f_value will be used if MessageSample gets changed to
            // use floats instead of strings for values
            result.append(QSharedPointer<MessageSample>(
                new MessageSample(label.toString(),
                    requested_value.toString(), d_timedate)
                )
            );
        }
    }
    else
    {
        QWARNING << "Wrong format of response: " << response;
    }
}

void APICallGetDataStreams::processResponse(bool error)
{
    if (!error && http.lastResponse().statusCode() == 200)
    {
//        QString test="\
//{\
// \"datastreams\" : [ {\
//     \"value_requested_at\" : \"2010-06-25T11:54:17.454020Z\",\
//     \"requested_value\" : \"999\",\
//     \"label\" : \"param1\"\
//  },\
//  {\
//     \"value_requested_at\" : \"2010-06-24T10:05:49.000000Z\",\
//     \"requested_value\" : \"0000017\",\
//     \"label\" : \"param2\"\
//  } ]\
//}";

//        parseJSONResponse(test, *receivedMessages);

        parseJSONResponse(http.readAll(), *receivedMessages);
    }

    semaphore->release();
}


APICallGetCommands::APICallGetCommands(
        QSharedPointer<SNGConnectAPI> &context,
        QSemaphore *semaphore,
        QQueue<QSharedPointer<Message> > *receivedMessages) :
    APICall(context), semaphore(semaphore),
    receivedMessages(receivedMessages) { }

/**
 * @brief APICallGetCommands::parseJSONResponse
 * Create MessageSample items in result. Can print some warnings if JSON is not understood.
 * @param response
 * @param result
 */
void APICallGetCommands::parseJSONResponse(
    QString response, QQueue< QSharedPointer<Message> > &result)
{
    QScriptValue sc, commands;
    QScriptEngine engine;
    sc = engine.evaluate("(" + response + ")");

    commands = sc.property("commands");

    if (commands.isArray())
    {
        QScriptValueIterator it(commands);

        while (it.hasNext())
        {
            it.next();
            // skip non-dictionary items in the 'commands' array
            if (!it.value().isObject()) {
                continue;
            }

            QScriptValue command =
                it.value().property("command");
            QScriptValue arguments =
                it.value().property("arguments");
            QMap< QString, QVariant > argumentsMap;

            if (!command.isValid())
            {
                QWARNING << "Wrong format of response: " << response;
                return;
            }

            if (arguments.isObject())
            {
                QScriptValueIterator it2(arguments);
                while (it2.hasNext()) {
                    it2.next();
                    argumentsMap[it2.name()] = it2.value().toVariant();
                }
            } else if (arguments.isValid()) {
                // 'arguments' is there but it's not an object!
                QWARNING << "Problem parsing arguments for command " <<
                    command.toString() << ": "<< arguments.toString();
                return;
            }

            result.append(QSharedPointer<MessageRequest>(
                new MessageRequest(command.toString(), argumentsMap))
            );
        }
    } else {
        QWARNING << "Wrong format of response: " << response;
    }
}

void APICallGetCommands::processResponse(bool error)
{
    if (!error /*&& http.lastResponse().statusCode() == 200*/)
    {
//        QString test="\
//{\
//    \"commands\" : [ \
//        {\"command\" : \"reboot\"},\
//        {\"command\" : \"upload_log\",\
//        \"arguments\" : { \
//            \"url\" :\"http://blah.com\",\
//            \"start\" : \"timestamp\"}\
//        } ]\
//}";
//        parseJSONResponse(test, *receivedMessages);

        parseJSONResponse(http.readAll(), *receivedMessages);
    }

    semaphore->release();
}

APICallSendLog::APICallSendLog(
            QSharedPointer<SNGConnectAPI> &context,
            MessageProxy *proxy):
    APICall(context), proxy(proxy)
{
    Q_ASSERT(proxy->isLocked());

    QSharedPointer<MessageResponse> response =
        proxy->object().staticCast<MessageResponse>();

    log_request_id = response->arguments["log_request_id"].toString();
    log_request_hash = response->arguments["log_request_hash"].toString();
}

QString APICallSendLog::getContent()
{
    QSharedPointer<MessageResponse> response =
        proxy->object().staticCast<MessageResponse>();

    return response->arguments["log"].toString();
}

void APICallSendLog::processResponse(bool error)
{
    if (!error && http.lastResponse().statusCode() == 200) {
        proxy->setProcessed();
    } else {
        proxy->setLocked(false);
        proxy->processingFailed();
    }
}
