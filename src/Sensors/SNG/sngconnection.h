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

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();

signals:
    void readyToRead();

public slots:
    void readFromSensor();
    void handleError(QAbstractSocket::SocketError);

private:

    SngConnection(Configurator*,int);

    // methods used when sending a message to sensor
    void sendMessage(Message&);

    bool translateMessageToFrame(Message&, SngFrame&);
    bool parseFrameType(QString&, SngFrameType&);
    bool translateFrameToMessage(SngFrame&, Message&);
    QString printMsg(char* msg);

    Configurator* conf;
    int no;

    QString commServerHostName;
    qint16 port;

    QTcpSocket* commServer;
    SngPhysicalAddress physicalAddress;

    SngMsgCreator msgCreator;
    SngMsgParser msgParser;

    QVector<Message> msgQue;
};

#endif // SNGCONNECTION_H
