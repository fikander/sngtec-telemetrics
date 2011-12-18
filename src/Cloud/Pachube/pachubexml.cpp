#include "pachubexml.h"
#include "src/Message/message.h"
#include <QDebug>

PachubeXml::PachubeXml(QString feed): feed(feed) {
    QDomElement eeml = xml.createElement("eeml");
    eeml.setAttribute("xmlns", "http://www.eeml.org/xsd/0.5.1");
    eeml.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    eeml.setAttribute("version", "0.5.1");
    eeml.setAttribute("xsi:schemaLocation", "http://www.eeml.org/xsd/0.5.1 http://www.eeml.org/xsd/0.5.1/0.5.1.xsd");
    xml.appendChild(eeml);

    environment = xml.createElement("environment");
    environment.setAttribute("id", feed);
    eeml.appendChild(environment);
    
}

//PachubeXml::PachubeXml(const PachubeXml &px): feed(px.feed), xml(px.xml), environment(px. environment){
//}


void PachubeXml::addData(const Message &message){
    environment.appendChild(messageToNode(message));
}

QDomDocument PachubeXml::getXml(){
    qDebug() << "got xml" << xml.toString();
    return xml;
}

inline
QDomElement PachubeXml::messageToNode(const Message &message){
    QDomElement data = xml.createElement("data");
    data.setAttribute("id", message.key);

    QDomElement current = xml.createElement("current_value");
    data.appendChild(current);

    QDomText value = xml.createTextNode(message.value);
    current.appendChild(value);
    
    return data;
}
