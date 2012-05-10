#include "cosmxml.h"
#include "Message/message.h"
#include <QDebug>
#include "cosmtime.h"

CosmXml::CosmXml(QString feed): feed(feed) {
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

//Cosm from xml
CosmXml::CosmXml(QString *xmlString) { 
    QString errorMsg = 0;
    int errorLine = 0;
    int errorColumn = 0;
    xml.setContent(*xmlString, false, &errorMsg, &errorLine, &errorColumn);
}


//CosmXml::CosmXml(const CosmXml &px): feed(px.feed), xml(px.xml), environment(px. environment){
//}


void CosmXml::addData(const Message &message){
    environment.appendChild(messageToNode(message));
}

QDomDocument CosmXml::getXml(){
    //qDebug() << "got xml" << xml.toString();
    return xml;
}

inline
QDomElement CosmXml::messageToNode(const Message &message){
    QDomElement data = xml.createElement("data");
    data.setAttribute("id", message.key);

    QDomElement current = xml.createElement("current_value");
    data.appendChild(current);

    QDomText value = xml.createTextNode(message.value);
    current.appendChild(value);
    
    return data;
}

inline
QDomNodeList CosmXml::getData() {
    return xml.elementsByTagName("data");
}

QVector<Message> CosmXml::getMessages() {
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
                Message message = Message(id, value);
                message.timestamp = CosmTime(timestamp).getDateTime();
                messages.push_back(message);
            }
        }
    }
    return messages;
}
                



