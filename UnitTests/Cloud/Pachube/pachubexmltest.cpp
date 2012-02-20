#include "pachubexmltest.h"
#include "src/Cloud/Pachube/pachubexml.h"
#include "src/Message/message.h"
#include <QVector>
#include <QString>


void PachubexmlTest::addingDataTest() {
    PachubeXml xml("");
    Message m1("key1", "value1");
    Message m2("key2", "value2");
    xml.addData(m1);
    xml.addData(m2);
    QVector<Message> result = xml.getMessages();
    
    QCOMPARE(2, result.size());
    QCOMPARE(result[0].key, m1.key);
    QCOMPARE(result[0].value, m1.value);
    QCOMPARE(result[1].key, m2.key);
    QCOMPARE(result[1].value, m2.value);

}

void PachubexmlTest::getMessagesTest() {
    QString xmlString = "<eeml version=\"0.5.1\" xsi:schemaLocation=\"http://www.eeml.org/xsd/0.5.1 http://www.eeml.org/xsd/0.5.1/0.5.1.xsd\"><environment updated=\"2011-12-20T14:03:04.579699Z\" id=\"41613\" creator=\"https://pachube.com/users/kaszynek\"><title>testfeed</title><feed>https://api.pachube.com/v2/feeds/41613.xml</feed><auto_feed_url>https://api.pachube.com/v2/feeds/41613.xml</auto_feed_url><status>frozen</status><description>test feed</description><private>false</private><tag>test</tag><location domain=\"physical\" exposure=\"\" disposition=\"fixed\"><name>Kara sea</name><lat>77.6206535033663</lat><lon>101.462860107422</lon></location><data id=\"key1\"><current_value at=\"2011-12-17T17:51:34.215211Z\">value1</current_value><max_value>100.0</max_value><min_value>11.0</min_value></data><data id=\"key2\"><current_value at=\"2011-12-20T14:03:04.574779Z\">value2</current_value></data></environment></eeml>";
    PachubeXml xml = PachubeXml::PachubeFromXml(xmlString);

    QVector<Message> result = xml.getMessages();
    QCOMPARE(2, result.size());
    QCOMPARE(result[0].key, QString("key1"));
    QCOMPARE(result[0].value, QString("value1"));
    QCOMPARE(result[1].key, QString("key2"));
    QCOMPARE(result[1].value, QString("value2"));
}

