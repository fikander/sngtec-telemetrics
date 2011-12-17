#ifndef SNGCONNECTION_H
#define SNGCONNECTION_H

#include "src/Sensors/devconnection.h"
#include "src/Sensors/SNG/sngphysicaladdress.h"
#include <QTcpSocket>
#include <QStringList>

#define SNG_FRAME_SIZE 14

class SngConnection : DevConnection
{
    Q_OBJECT

public:
    virtual DevConnection* create(Configurator *config);
    virtual DevConnection* clone(Configurator *config);
    virtual ~SngConnection();

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();

signals:
    void readyToRead();

public slots:
    void readFromSensor();
    void handleError(QAbstractSocket::SocketError);

private:
    enum SngFrameType
    {
        OnOff, Dimm, Time, Date, Temp, Value
    };


    SngConnection(Configurator*);

    // methods used when sending a message to sensor
    void sendMessage(Message&);
    void sendFrame(SngPhysicalAddress&, SngPhysicalAddress&, SngFrameType frameType, QString& value);
    void setBeginAndEndOfFrame(char*);
    void setFrameType(SngFrameType, char*);
    void setSrcAddr(SngPhysicalAddress&, char*);
    void setDestAddr(SngPhysicalAddress&, char*);
    void setAddr(SngPhysicalAddress&, char*);

    //set value and methods used by one
    void setValue(SngFrameType, char*, QString&);
    void parseOnOff(char*, QString&);   // correct values: on, off
    void parseDimm(char*, QString&);    // inc, endInc, dec, endDec
    void parseTime(char*, QString&);    // hh:mm:ss
    void parseDate(char*, QString&);    // dd.mm.yyyy
    void parseTemp(char*, QString&);    // [-]A.B , where A, B are from [0-255], minus optionaly
    void parseValue(char*, QString&);   // 0-255



    Configurator* conf;

    QString commServerHostName;
    qint16 port;

    QTcpSocket* commServer;
    SngPhysicalAddress physicalAddress;
    SngPhysicalAddress defaultDest;

    QVector<Message> msgQue;
};

#endif // SNGCONNECTION_H
