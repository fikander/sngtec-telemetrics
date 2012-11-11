#include <QStringList>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QVariant>

#include "debug.h"
#include "message.h"


void Message::getUnlockedMessages(
        QList< QSharedPointer<Message> > &messages,
        MessageType type,
        bool lock,
        QList< QSharedPointer<Message> > &result)
{
    foreach(QSharedPointer<Message> msg, messages)
    {
        if (msg->getType() == type && !msg->isLocked())
        {
            result.append(msg);
            msg->setLocked(lock);
        }
    }
}

void  MessageSample::takeMessagesByDatastream(
        QList< QSharedPointer<Message> > &messages,
        QString datastream,
        QList< QSharedPointer<MessageSample> > &result)
{
    QList<int> toTake;

    int i=0;
    foreach(QSharedPointer<Message> msg, messages)
    {
        if (msg->getType() == MsgSample)
        {
            QSharedPointer<MessageSample> s = msg.staticCast<MessageSample>();
            if (s->key == datastream)
                toTake.append(i);
        }
        ++i;
    }

    int taken = 0;
    foreach(int t, toTake)
    {
        result.append(messages.takeAt(t - taken++).staticCast<MessageSample>());
    }
}

Message::Message(QDateTime timestamp) :
    timestamp(timestamp), processed(false), locked(false), failCount(0)
{
}

Message::~Message()
{
    //QDEBUG << "Message ~Destructor";
}

MessageSample::MessageSample() :
    Message(QDateTime::currentDateTime())
{
}

MessageSample::MessageSample(QString k, QString v) :
    Message(QDateTime::currentDateTime())
{
    key = k;
    value = v;
}

MessageSample::MessageSample(QString k, QString v, QDateTime t) :
    Message(t)
{
    key = k;
    value = v;
}


QString MessageSample::toString()
{
    QString s = "(" + key + "," + value + "," + timestamp.toString() + ")";
    return s;
}

MessageEvent::MessageEvent(QString message, QString type, int id) :
    Message(QDateTime::currentDateTime()),
    message(message), type(type), id(id)
{
}

MessageRequest::MessageRequest(QString command, QMap<QString, QVariant> arguments) :
    Message(QDateTime::currentDateTime()),
    command(command), arguments(arguments)
{
}

QString MessageRequest::toString()
{
    QString result = "Request :" + command + ": ";
    foreach(QString key, arguments.keys())
    {
        result += key + "=" + arguments[key].toString() + ", ";
    }
    return result;
}

MessageResponse::MessageResponse(QString command, QMap<QString, QVariant> arguments) :
    Message(QDateTime::currentDateTime()),
    command(command), arguments(arguments)
{
}

QString MessageResponse::toString()
{
    QString result = "Request :" + command + ": ";
    foreach(QString key, arguments.keys())
    {
        result += key + "=" + arguments[key].toString() + ", ";
    }
    return result;
}
