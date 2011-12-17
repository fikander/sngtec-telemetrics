#include "sngconnection.h"

SngConnection::SngConnection(Configurator & config) :
    commServerHostName("localhost"), port(8800),
    physicalAddress("2.4.7"), defaultDest("1.2.3")
{
    this->conf = &config;
    commServer = new QTcpSocket(this);

    connect(commServer, SIGNAL(readyRead()), this, SLOT(readFromSensor()));
    connect(commServer, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));

    commServer->connectToHost(commServerHostName, port);
}

SngConnection::~SngConnection()
{
}

DevConnection* SngConnection::create(Configurator &config)
{
    return new SngConnection(config);
}

DevConnection* SngConnection::clone(Configurator &config)
{
    return new SngConnection(config);
}

void SngConnection::write(QVector<Message> messages)
{
    for (int i = 0; i < messages.size(); i++)
    {
        sendMessage(messages[i]);
    }

}

QVector<Message> SngConnection::readAll()
{
    QVector<Message> res = QVector<Message>(msgQue);
    msgQue.clear();
    return res;
}

void SngConnection::readFromSensor()
{

}

void SngConnection::handleError(QAbstractSocket::SocketError error)
{
    qDebug() << "Error with CommServer: " << error;
}

void SngConnection::sendMessage(Message& msg)
{
    // TODO get settings from config
    sendFrame(physicalAddress, defaultDest, Temp, msg.value);
}

void SngConnection::sendFrame(SngPhysicalAddress &src, SngPhysicalAddress &dest,
                              SngFrameType frameType, QString &value)
{
}






