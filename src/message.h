#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QDateTime>

class Message : public QObject {
public:
    Message();
    QString key;
    QString value;
    QDateTime timestamp;
};

#endif // MESSAGE_H
