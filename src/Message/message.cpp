#include <QStringList>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <iostream>

#include "debug.h"
#include "message.h"


MessageProxy::MessageProxy(QSharedPointer<Message> &message) :
    message(message), processed(false), locked(false), failCount(0) { }

MessageProxy::~MessageProxy()
{
    //std::cerr << "~MessageProxy" << std::endl;
}


void MessageProxy::getUnlockedMessages(
            QList< MessageProxy* > &proxies,
            Message::MessageType type,
            QList< MessageProxy* > &result)
{
    foreach(MessageProxy* proxyPtr, proxies) {
        MessageProxy &proxy = *proxyPtr;
        if (proxy->getType() == type && !proxy.isLocked()) {
            result.append(proxyPtr);
            proxy.setLocked(true);
        }
    }
}


Message::Message(QDateTime timestamp) :
    timestamp(timestamp) { }


Message::~Message()
{
    std::cerr << "~Message" << std::endl;
}


MessageSample::MessageSample() :
    Message(QDateTime::currentDateTime()) { }


MessageSample::MessageSample(QString k, QString v) :
    Message(QDateTime::currentDateTime()), key(k), value(v) { }


MessageSample::MessageSample(QString k, QString v, QDateTime t) :
    Message(t), key(k), value(v) { }


QString MessageSample::toString() const
{
    return "(" + key + "," + value + "," + timestamp.toString() + ")";
}


MessageEvent::MessageEvent(QString message, QString type, int id) :
    Message(QDateTime::currentDateTime()),
    message(message), type(type), id(id) { }


QString MessageEvent::toString() const
{
    return "[" + type + ":"+ QString::number(id) +"] " + message;
}


MessageRequest::MessageRequest(QString command, QMap<QString, QVariant> arguments) :
    Message(QDateTime::currentDateTime()),
    command(command), arguments(arguments) { }


QString MessageRequest::toString() const
{
    QString result = "Request :" + command + ": ";
    foreach(QString key, arguments.keys()) {
        result += key + "=" + arguments[key].toString() + ", ";
    }
    return result;
}


MessageResponse::MessageResponse(QString command, QMap<QString, QVariant> arguments) :
    Message(QDateTime::currentDateTime()),
    command(command), arguments(arguments) {}


QString MessageResponse::toString() const
{
    QString result = "Request :" + command + ": ";
    foreach(QString key, arguments.keys()) {
        result += key + "=" + arguments[key].toString() + ", ";
    }
    return result;
}
