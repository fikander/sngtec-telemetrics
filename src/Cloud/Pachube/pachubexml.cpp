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

//Pachube from xml
PachubeXml::PachubeXml(QString *xmlString) { 
    xml.setContent(*xmlString, false);
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

inline
QDomNodeList PachubeXml::getData() {
    return xml.elementsByTagName("data");
}

QVector<Message> PachubeXml::getMessages() {
    QDomNodeList dataNodes = getData();
    QVector<Message> messages;
    for(int i = 0; i < dataNodes.count(); ++i) {
        QDomNode data = dataNodes.at(i);
        if(data.hasAttributes() && data.hasChildNodes()) {
            QString id = data.attributes().namedItem("id").toAttr().value();
            QDomElement current_value =  data.firstChildElement("current_value");
            if(!current_value.isNull()) {
                QString timestamp = current_value.attribute("at");
                QString value = current_value.text();
                messages.push_back(Message(id, value));
            }
        }
    }
    return messages;
}
                



