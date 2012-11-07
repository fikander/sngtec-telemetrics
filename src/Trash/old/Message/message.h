#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDebug>
#include <QObject>
#include <QDateTime>

class Message
{

public:
    enum MessageType { MsgBase, MsgSample, MsgEvent, MsgRequest, MsgResponse };

    Message(QDateTime timestamp);
    virtual ~Message();
    virtual MessageType getType() { return MsgBase; }
    virtual QString toString() { return "base"; }

    QDateTime timestamp;

    // message is marked as processed by cloud or device after it's been successfully sent
    // message is marked as locked when cloud is attempting to process it.
    // used to avoid trying to process single message more than once
    bool processed, locked;
    int failCount;
    bool isLocked() { return locked; }
    void setLocked(bool value) { locked = value; }
    bool isProcessed() { return processed; }
    void setProcessed() { processed = true; }
    void processingFailed() { failCount++; }
    int getProcessingFailed() { return failCount; }

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

/**
 * @brief The MessageEvent class
 * Sent FROM devices whenever something extraordinary happens, e.g. alert
 */
class MessageEvent : public Message
{
public:
    MessageEvent();
    virtual MessageType getType() { return MsgEvent; }
    virtual QString toString() { return "event"; }
};

/**
 * @brief The MessageRequest class
 * Requests/commands sent from the cloud TO devices.
 */
class MessageRequest : public Message
{
public:
    MessageRequest(QString command, QMap<QString, QVariant> arguments);
    virtual MessageType getType() { return MsgRequest; }
    virtual QString toString();

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
    virtual MessageType getType() { return MsgResponse; }
    virtual QString toString();

    QString command;
    QMap< QString, QVariant > arguments;
};

#endif // MESSAGE_H
