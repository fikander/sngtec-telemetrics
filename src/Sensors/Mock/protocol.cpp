#include "protocol.h"

#include <QDebug>

Protocol::Protocol()
{
    test = 1;
}

int Protocol::readTest(){
    return test;
}

QString Protocol::read(){
    QString p = QString("ProtocolRead");
    return p;
}

int Protocol::write(QString string){
    qDebug() << string;
    return 0;
}
