#include "debug.h"

#include <QNetworkRequest>
//http://qtwiki.org/Parsing_JSON_with_QT_using_standard_QT_library
//http://qt-project.org/doc/note_revisions/69/106/view
#include <QtScript/QScriptEngine>

#include "SNGConnectAPI.h"

SNGConnectAPI::SNGConnectAPI(KeyValueMap &config)
{
    baseUrl = config["baseurl"];
    Q_ASSERT(baseUrl.isValid());

    feed = config["feed"];
}

bool SNGConnectAPI::makeHttpRequest(QString method, QString api, QString contents)
{
    QHttpRequestHeader header(method, QUrl::toPercentEncoding(api));

    header.setValue("Host", baseUrl.host());
    header.setContentType("application/xml");

    http.setHost(baseUrl.host(),
        baseUrl.scheme() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp);
    http.request(header, contents);
}

/*
POST api/v1/feed/{id}/events.json
{
"type": "alarm_on|alarm_off|information|system_error|system_warning",
"id": "1",
"timestamp": "<timestamp>"
"message": "something’s gone wrong"
}
*/
void SNGConnectAPI::sendEvents(QList< QSharedPointer<Message> > &events)
{
    QString contents = "{}";
    makeHttpRequest("POST", "api/v1/feed/" + feed + "/events.json", contents);
}
/*
PUT api/v1/feeds/{id}/datastreams/{id}.json
{
  "datapoints":[
    {"at":"2010-05-20T11:01:43Z","value":"294"},
    {"at":"2010-05-20T11:01:44Z","value":"295"},
    {"at":"2010-05-20T11:01:45Z","value":"296"},
    {"at":"2010-05-20T11:01:46Z","value":"297"}
  ]
}
*/
void SNGConnectAPI::sendDatastreamSamples(QString datastream, QList< QSharedPointer<MessageSample> > &samples)
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
