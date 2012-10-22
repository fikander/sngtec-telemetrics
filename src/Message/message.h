#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDebug>
#include <QObject>
#include <QDateTime>


class Message
{

public:
    enum MessageType { MsgBase, MsgSample, MsgEvent, MsgRequest };

    Message(QDateTime timestamp);
    virtual ~Message() {}
    virtual MessageType getType() = 0;
    virtual QString toString() = 0;

    QDateTime timestamp;

    static QList< QSharedPointer<Message> > takeMessages(QList<QSharedPointer<Message> > &messages, MessageType type);
};


class MessageSample : public Message
{
public:
    MessageSample();
    MessageSample(QString key, QString val);
    MessageSample(QString key, QString val, QDateTime timestamp);

    virtual MessageType getType() { return MsgSample; }

    QString key;
    QString value;

    QByteArray toByteArray();
    bool fromByteArray(QByteArray&);
    virtual QString toString();

    static QList<QSharedPointer<MessageSample> > takeMessagesByDatastream(QList<QSharedPointer<Message> > &messages, QString datastream);
};

class MessageEvent : public Message
{
public:
    MessageEvent();
    virtual MessageType getType() { return MsgEvent; }
    virtual QString toString() { return "event"; }
};

#endif // MESSAGE_H
