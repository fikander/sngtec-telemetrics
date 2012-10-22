#ifndef SNGCONNECTAPI_H
#define SNGCONNECTAPI_H

#include <QSharedPointer>
#include <QHttp>

#include "KeyValueMap.h"

class SNGConnectAPI
{
public:
    SNGConnectAPI(KeyValueMap &config);

    void sendEvents(QList< QSharedPointer<Message> > &events);
    void sendDatastreamSamples(QString datastream, QList< QSharedPointer<MessageSample> > &samples);

private:
    QUrl baseUrl;
    QString feed;
};

#endif // SNGCONNECTAPI_H
