#ifndef PACHUBE_XML_H
#define PACHUBE_XML_H

#include <QDomDocument>
#include <QString>
#include <QDomElement>
#include <QVector>
#include <QDomNodeList>
#include "src/Message/message.h"
#include <QDebug>

class Message;

class PachubeXml {
public:
    PachubeXml(QString feed);
    static PachubeXml PachubeFromXml(QString xml);
    QVector<Message> getMessages();
    //PachubeXml(const PachubeXml &);
    QDomDocument getXml();
    void addData(const Message &);

protected:
    PachubeXml(QString *xml);

private:
    QDomElement messageToNode(const Message &);
    QDomNodeList getData();

    QString feed;
    QDomDocument xml;
    QDomElement environment;

};

inline
PachubeXml PachubeXml::PachubeFromXml(QString xml){
    qDebug() << xml;
    return PachubeXml(&xml);
}

#endif
