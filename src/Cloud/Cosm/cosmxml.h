#ifndef COSM_XML_H
#define COSM_XML_H

#include <QDomDocument>
#include <QString>
#include <QDomElement>
#include <QVector>
#include <QDomNodeList>
#include "Message/message.h"
#include <QDebug>

class Message;

class CosmXml {
public:
    CosmXml(QString feed);
    static CosmXml CosmFromXml(QString xml);
    QVector<Message> getMessages();
    //CosmXml(const CosmXml &);
    QDomDocument getXml();
    void addData(const Message &);

protected:
    CosmXml(QString *xml);

private:
    QDomElement messageToNode(const Message &);
    QDomNodeList getData();

    QString feed;
    QDomDocument xml;
    QDomElement environment;

};

inline
CosmXml CosmXml::CosmFromXml(QString xml){
    //qDebug() << xml;
    return CosmXml(&xml);
}

#endif
