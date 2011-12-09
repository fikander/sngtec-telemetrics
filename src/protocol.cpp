#include "protocol.h"

#include <QDebug>

protocol::protocol()
{
    test = 1;
}

int protocol::readTest(){
    return test;
}

QString protocol::read(){
    QString p = QString("ProtocolRead");
    return p;
}

int protocol::write(QString string){
    qDebug() << string;
    return 0;
}
