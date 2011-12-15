#ifndef PACHUBE_XML_H
#define PACHUBE_XML_H

#include <QDomDocument>
#include <QString>
#include <QDomElement>

class Message;

class PachubeXml {
public:
    PachubeXml(QString feed);
    QDomDocument getXml();
    void addData(const Message &);

private:
    QDomElement messageToNode(const Message &);

    QString feed;
    QDomDocument xml;
    QDomElement environment;

};

#endif
