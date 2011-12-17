#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QDateTime>

class Message {
public:
    Message();
    Message(QString key, QString val);
    QString key;
    QString value;
    QDateTime timestamp;
};

#endif // MESSAGE_H
