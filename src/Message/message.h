#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDebug>
#include <QObject>
#include <QDateTime>

class Message
{
public:
    enum MessageType {
        MsgBase,
        MsgSample,
        MsgEvent,
        MsgRequest,
        MsgResponse
    };

    Message(QDateTime timestamp);
    virtual ~Message();
    virtual MessageType getType() const { return MsgBase; }
    virtual QString toString() const { return "base"; }

    QDateTime timestamp;
};


class MessageProxy
{
public:
    MessageProxy(QSharedPointer<Message> &message);
    ~MessageProxy();

    // message is marked as locked when cloud is attempting to process it
    // used to avoid trying to process single message more than once
    bool isLocked()
        { return locked; }
    void setLocked(bool value)
        { locked = value; }
    // message is marked as processed by cloud or device after it's been
    // successfully sent
    bool isProcessed()
        { return processed; }
    void setProcessed()
        { processed = true; }
    // messages will be discarded after several attemps to process
    // (this is configurable per Cloud)
    void processingFailed()
        { failCount++; }
    int getProcessingFailed()
        { return failCount; }

    QString toString() const
        { return message->toString(); }

    QSharedPointer<Message> operator->() const
        { return message; }
    QSharedPointer<Message> &object()
        { return message; }

protected:
    QSharedPointer<Message> message;

    bool processed, locked;
    int failCount;

public:
    static void getUnlockedMessages(
            QList< MessageProxy* > &proxies,
            Message::MessageType type,
            QList< MessageProxy* > &result);

};


class MessageSample : public Message
{
public:
    MessageSample();
    MessageSample(QString key, QString val);
    MessageSample(QString key, QString val, QDateTime timestamp);
    virtual MessageType getType() const { return MsgSample; }

    QString key;
    QString value;

    virtual QString toString() const;
};

/**
 * @brief The MessageEvent class
 * Sent FROM devices whenever something extraordinary happens, e.g. alert
 */
class MessageEvent : public Message
{
public:
    MessageEvent(QString message, QString type, int id);
    virtual MessageType getType() const { return MsgEvent; }
    virtual QString toString() const;

    QString message;
    //"type": "alarm_on|alarm_off|information|system_error|system_warning"
    QString type;
    int id;
};

/**
 * @brief The MessageRequest class
 * Requests/commands sent from the cloud TO devices.
 */
class MessageRequest : public Message
{
public:
    MessageRequest(QString command, QMap<QString, QVariant> arguments);
    virtual MessageType getType() const { return MsgRequest; }
    virtual QString toString() const;

    QString command;
    QMap< QString, QVariant > arguments;
};

/**
 * @brief The MessageResponse class
 * Response to the MsgRequest
 */
class MessageResponse: public Message
{
public:
    MessageResponse(QString command, QMap<QString, QVariant> arguments);
    virtual MessageType getType() const { return MsgResponse; }
    virtual QString toString() const;

    QString command;
    QMap< QString, QVariant > arguments;
};

#endif // MESSAGE_H
