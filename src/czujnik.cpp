#include "czujnik.h"
#include <QDebug>

Czujnik::Czujnik() {
    myProtocol = new protocol();
}

QString Czujnik::readMesg(){
    QString mesg = myProtocol->read();
    qDebug() << mesg;
    return mesg;
}

int Czujnik::sendMesg(QString mesg){
    if (myProtocol->write(mesg))
        return 1;
    return 0;
}

int Czujnik::connectDevice(DevProxy* device){
    myDevice = device;
    //Device ma wiedziec kiedy przyjdzie odpowiedz
    connect(this, SIGNAL(readyRead()), device, SLOT(readDevice()));
    readyRead();
    return 0;
}

qint64 Czujnik::writeData(const char *data, qint64 len){
    qint64 a = 1;
    return a;
}

qint64 Czujnik::readData(char *data, qint64 maxlen){
    return maxlen;
}


