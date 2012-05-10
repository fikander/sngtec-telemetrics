#include <QCoreApplication>
#include "cosm.h"
#include <QTime>
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QDomDocument xml; 
    QString error;
    int err_line, err_column;
    if(!xml.setContent(QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?> <eeml xmlns=\"http://www.eeml.org/xsd/0.5.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"0.5.1\" xsi:schemaLocation=\"http://www.eeml.org/xsd/0.5.1 http://www.eeml.org/xsd/0.5.1/0.5.1.xsd\"> <environment> <title>testfeed</title> <feed>https://api.cosm.com/v2/feeds/41613.xml</feed> <description>test feed</description> <tag>test</tag> <data id=\"1\"> <current_value>100</current_value> </data> </environment> </eeml>")
        , &error, &err_line, &err_column)){
        qDebug("error on line %d column %d", err_line, err_column);
        qDebug() << error;
    }
    else { 
        Cosm p("xEc6VTbLaHKgmXMB24UCCXROcXm_hr2FWCFarVhHGtg", "/v2/feeds/41613.xml", xml);

        QTime dieTime = QTime::currentTime().addSecs(10);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    }   
    return 0;
}
