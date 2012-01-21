#ifndef SNGCONNECTION_H
#define SNGCONNECTION_H

#include "Sensors/devconnection.h"
#include "sngphysicaladdress.h"
#include "sngframe.h"
#include "sngmsgcreator.h"
#include <QTcpSocket>
#include <QStringList>

#define SNG_FRAME_SIZE 14

class SngConnection : public DevConnection
{
    Q_OBJECT

public:
    SngConnection();
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

    SngConnection(Configurator*);

    // methods used when sending a message to sensor
    void sendMessage(Message&);

    SngFrame translateMessageToFrame(Message&);
    SngFrameType parseFrameType(QString&);

    Configurator* conf;

    QString commServerHostName;
    qint16 port;

    QTcpSocket* commServer;
    SngPhysicalAddress physicalAddress;
    SngPhysicalAddress defaultDest;

    SngMsgCreator msgCreator;

    QVector<Message> msgQue;
};

#endif // SNGCONNECTION_H
