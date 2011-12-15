#include "pachubexml.h"
#include "../../Message/message.h"

PachubeXml::PachubeXml(QString feed): feed(feed) {
    QDomElement eeml = xml.createElement("eeml");
    eeml.setAttribute("version", "0.5.1");
    eeml.setAttribute("xsi:schemaLocation",
            "http://www.eeml.org/xsd/0.5.1 http://www.eeml.org/xsd/0.5.1/0.5.1.xsd");
    xml.appendChild(eeml);

    environment = xml.createElement("environment");
    environment.setAttribute("id", feed);
    eeml.appendChild(environment);
    
}

void PachubeXml::addData(const Message &message){
    environment.appendChild(messageToNode(message));
}

QDomDocument PachubeXml::getXml(){
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
