#ifndef SNGCONNECTION_H
#define SNGCONNECTION_H

#include "Sensors/devconnection.h"
#include "sngphysicaladdress.h"
#include "sngframe.h"
#include "sngmsgcreator.h"
#include "sngmsgparser.h"
#include <QTcpSocket>
#include <QStringList>

#define SNG_FRAME_SIZE 14

static const QString addresKey = "address";
static const QString portKey = "port";
static const QString physicalAddressKey = "physicalAddress";
static const QString separator = "__";

class SngConnection : public DevConnection
{
    Q_OBJECT

public:
    SngConnection();
    virtual DevConnection* create(Configurator *config, int no);
    virtual DevConnection* clone(Configurator *config, int no);
    virtual ~SngConnection();

    virtual void write(QVector<MessageSample>);
    virtual QVector<MessageSample> readAll();

signals:
    void readyToRead();

public slots:
    void readFromSensor();
    void handleError(QAbstractSocket::SocketError);

private:
    SngConnection(Configurator*,int);

    // methods used when sending a message to sensor
    void sendMessage(MessageSample&);

    bool translateMessageToFrame(MessageSample&, SngFrame&);
    bool parseFrameType(QString&, SngFrameType&);
    bool translateFrameToMessage(SngFrame&, MessageSample&);
    QString printMsg(char* msg);

    // handle SNG 'heartbeat'
    bool checkHeartBeat(char*msg);
    int heartBeatNo;

    Configurator* conf;
    int no;

    QString commServerHostName;
    qint16 port;

    QTcpSocket* commServer;
    SngPhysicalAddress physicalAddress;

    SngMsgCreator msgCreator;
    SngMsgParser msgParser;

    QVector<MessageSample> msgQue;
};

#endif // SNGCONNECTION_H
