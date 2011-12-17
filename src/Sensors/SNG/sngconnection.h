#ifndef SNGCONNECTION_H
#define SNGCONNECTION_H

#include "src/Sensors/devconnection.h"
#include "src/Sensors/SNG/sngphysicaladdress.h"
#include <QTcpSocket>

#define SNG_FRAME_SIZE 14

class SngConnection : DevConnection
{
    Q_OBJECT

public:
    virtual DevConnection* create(Configurator &config);
    virtual DevConnection* clone(Configurator &config);
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


    SngConnection(Configurator&);

    void sendMessage(Message&);
    void sendFrame(SngPhysicalAddress&, SngPhysicalAddress&, SngFrameType frameType, QString& value);
    void setBeginAndEndOfFrame(char*);
    void setFrameType(SngFrameType, char*);
    void setSrcAddr(SngPhysicalAddress&, char*);
    void setDestAddr(SngPhysicalAddress&, char*);


    Configurator* conf;

    QString commServerHostName;
    qint16 port;

    QTcpSocket* commServer;
    SngPhysicalAddress physicalAddress;
    SngPhysicalAddress defaultDest;

    QVector<Message> msgQue;
};

#endif // SNGCONNECTION_H
