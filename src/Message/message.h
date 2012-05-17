#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QDateTime>

class Message {
public:
    Message();
    Message(QString key, QString val);
    Message(QString key, QString val, QDateTime timestamp);
    QString key;
    QString value;
    QDateTime timestamp;

    QByteArray toByteArray();
    bool fromByteArray(QByteArray&);
    QString toString();
};

#endif // MESSAGE_H
