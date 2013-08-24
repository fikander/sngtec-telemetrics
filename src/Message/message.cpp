#include <QStringList>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QVariant>

#include "debug.h"
#include "message.h"


MessageProxy::MessageProxy(QSharedPointer<Message> &message) :
    message(message), processed(false), locked(false), failCount(0) { }


void MessageProxy::getUnlockedMessages(
            QList< MessageProxy* > &proxies,
            MessageType type,
            QList< MessageProxy* > &result)
{
    foreach(MessageProxy* proxyPtr, proxies) {
        MessageProxy &proxy = *proxyPtr;
        if (proxy->getType() == type && !proxy->isLocked()) {
            result.append(proxyPtr);
            proxyPtr->setLocked(true);
        }
    }
    // foreach(QSharedPointer<Message> msg, messages) {
    //     if (msg->getType() == type && !msg->isLocked()) {
    //         result.append(msg);
    //         msg->setLocked(lock);
    //     }
    // }
}


Message::Message(QDateTime timestamp) :
    timestamp(timestamp) { }


Message::~Message()
{
    //QDEBUG << "Message ~Destructor";
}


MessageSample::MessageSample() :
    Message(QDateTime::currentDateTime()) { }


MessageSample::MessageSample(QString k, QString v) :
    Message(QDateTime::currentDateTime()), key(k), value(v) { }


MessageSample::MessageSample(QString k, QString v, QDateTime t) :
    Message(t), key(k), value(v) { }


MessageSample* MessageSample::clone() const
{
    return new MessageSample(*this);
}


QString MessageSample::toString() const
{
    return "(" + key + "," + value + "," + timestamp.toString() + ")";
}


MessageEvent::MessageEvent(QString message, QString type, int id) :
    Message(QDateTime::currentDateTime()),
    message(message), type(type), id(id) { }


MessageEvent* MessageEvent::clone() const
{
    return new MessageEvent(*this);
}


QString MessageEvent::toString() const
{
    return "[" + type + ":"+ QString::number(id) +"] " + message;
}


MessageRequest::MessageRequest(QString command, QMap<QString, QVariant> arguments) :
    Message(QDateTime::currentDateTime()),
    command(command), arguments(arguments) { }


MessageRequest* MessageRequest::clone() const
{
    return new MessageRequest(*this);
}


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


MessageResponse* MessageResponse::clone() const
{
    return new MessageResponse(*this);
}


QString MessageResponse::toString() const
{
    QString result = "Request :" + command + ": ";
    foreach(QString key, arguments.keys()) {
        result += key + "=" + arguments[key].toString() + ", ";
    }
    return result;
}
