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
    virtual ~Message();
    virtual MessageType getType() = 0;
    virtual QString toString() = 0;

    QDateTime timestamp;

    // message is marked as processed by cloud or device after it's been successfully sent
    bool processed, locked;
    bool isLocked() { return locked; }
    void setLocked(bool value) { locked = value; }
    bool isProcessed() { return processed; }
    void setProcessed() { processed = true; }

    static void getUnlockedMessages(
            QList< QSharedPointer<Message> > &messages,
            MessageType type,
            bool lock,
            QList< QSharedPointer<Message> > &result);
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

    static void takeMessagesByDatastream(
            QList< QSharedPointer<Message> > &messages,
            QString datastream,
            QList< QSharedPointer<MessageSample> > &result);
};

class MessageEvent : public Message
{
public:
    MessageEvent();
    virtual MessageType getType() { return MsgEvent; }
    virtual QString toString() { return "event"; }
};

class MessageRequest : public Message
{
public:
    MessageRequest();
    virtual MessageType getType() { return MsgRequest; }
    virtual QString toString() { return "request"; }
};

#endif // MESSAGE_H
