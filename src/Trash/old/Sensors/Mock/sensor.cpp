#include "czujnik.h"
#include <QDebug>

Sensor::Sensor() {
    myProtocol = new Protocol();
}

QString Sensor::readMesg(){
    QString mesg = myProtocol->read();
    qDebug() << mesg;
    return mesg;
}

int Sensor::sendMesg(QString mesg){
    if (myProtocol->write(mesg))
        return 1;
    return 0;
}

int Sensor::connectDevice(DevProxy* device){
    myDevice = device;
    //Device ma wiedziec kiedy przyjdzie odpowiedz
    connect(this, SIGNAL(readyRead()), device, SLOT(readDevice()));
    readyRead();
    return 0;
}

qint64 Sensor::writeData(const char *data, qint64 len){
    qint64 a = 1;
    return a;
}

qint64 Sensor::readData(char *data, qint64 maxlen){
    return maxlen;
}


