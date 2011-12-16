#include "sngconnection.h"

SNGconnection::SNGconnection(Configurator & config) :
    commServerHostName("localhost"), port(8800)
{
    this->conf = &config;
    commServer = new QTcpSocket(this);

    connect(commServer, SIGNAL(readyRead()), this, SLOT(readFromSensor()));
    connect(commServer, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));

    commServer->connectToHost(commServerHostName, port);
}

SNGconnection::~SNGconnection()
{
}

DevConnection* SNGconnection::create(Configurator &config)
{
    return new SNGconnection(config);
}

DevConnection* SNGconnection::clone(Configurator &config)
{
    return new SNGconnection(config);
}

void SNGconnection::write(QVector<Message> messages)
{
    //it = messages.constBegin();
}

QVector<Message> SNGconnection::readAll()
{
    QVector<Message> res;
    return res;
}

void SNGconnection::readFromSensor()
{
}

void SNGconnection::handleError(QAbstractSocket::SocketError error)
{
    qDebug() << "Error with CommServer: " << error;
}




