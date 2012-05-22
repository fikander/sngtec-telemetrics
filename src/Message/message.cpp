#include "message.h"
#include <QStringList>
#include <QDebug>
#include <QString>
#include <QByteArray>

Message::Message() {
    timestamp = QDateTime::currentDateTime();
}

Message::Message(QString k, QString v) {
    key = k;
    value = v;
    timestamp = QDateTime::currentDateTime();
}

Message::Message(QString k, QString v, QDateTime t) {
    key = k;
    value = v;
    timestamp = t; 
}

QByteArray Message::toByteArray()
{
    QString s = "##" + key + "#" + value + "#" + timestamp.toString() + "##";
    QByteArray res;
    res.append(s);
    return res;
}

bool Message::fromByteArray(QByteArray& buf)
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

QString Message::toString()
{
    QString s = "(" + key + "," + value + "," + timestamp.toString() + ")";
    return s;
}
