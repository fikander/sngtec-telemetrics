#include "message.h"
#include <QStringList>
#include <QDebug>
#include <QString>
#include <QByteArray>


QList< QSharedPointer<Message> > Message::takeMessages(QList< QSharedPointer<Message> > &messages, MessageType type)
{
    QList< QSharedPointer<Message> > result;
    QList<int> toTake;

    int i = 0;
    foreach(QSharedPointer<Message> msg, messages)
    {
        if (msg->getType() == type)
            toTake.append(i);
        ++i;
    }

    int taken = 0;
    foreach(int t, toTake)
    {
        result.append(messages.takeAt(t - taken++));
    }

    return result;
}

QList< QSharedPointer<MessageSample> > MessageSample::takeMessagesByDatastream(QList< QSharedPointer<Message> > &messages, QString datastream)
{
    QList< QSharedPointer<MessageSample> > result;
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

    return result;
}


MessageSample::MessageSample() {
    timestamp = QDateTime::currentDateTime();
}

MessageSample::MessageSample(QString k, QString v) {
    key = k;
    value = v;
    timestamp = QDateTime::currentDateTime();
}

MessageSample::MessageSample(QString k, QString v, QDateTime t) {
    key = k;
    value = v;
    timestamp = t; 
}

QByteArray MessageSample::toByteArray()
{
    QString s = "##" + key + "#" + value + "#" + timestamp.toString() + "##";
    QByteArray res;
    res.append(s);
    return res;
}

bool MessageSample::fromByteArray(QByteArray& buf)
{
    QString s = QString(buf);
    QStringList lista = s.split("#",QString::SkipEmptyParts);

    if (/*s[0] != '#' || s[1] != '#' || s[s.size()-1] != '#' || s[s.size()-2] != '#' ||*/ lista.size() != 3)
    {
        //qDebug() << "Message::fromString: bad message. got: " << s;
        return false;
    }

    key = lista[0];
    value = lista[1];
    timestamp = QDateTime::fromString(lista[2]);
    return true;
}

QString MessageSample::toString()
{
    QString s = "(" + key + "," + value + "," + timestamp.toString() + ")";
    return s;
}
