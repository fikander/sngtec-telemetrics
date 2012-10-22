#include "topologysensor.h"
#include <QString>
#include <QStringList>
#include <QByteArray>


TopologySensor::TopologySensor(Configurator * config, int no) :
    tcpServer(0), clientConnection(0), port(0)
{
    port = config->deviceTranslate(no, "port").toInt();

    tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleConnection()));

    if (!tcpServer->listen(QHostAddress::Any, port)) {
        qCritical() << "TopologySensor: Unable to start the server: " << tcpServer->errorString();
        return;
    }
}


TopologySensor::TopologySensor() {
}

DevConnection* TopologySensor::create(Configurator *config, int no)
{
    return new TopologySensor(config, no);
}

DevConnection* TopologySensor::clone(Configurator *config, int no)
{
    return new TopologySensor(config, no);
}

TopologySensor::~TopologySensor()
{
}

void TopologySensor::write(QVector<MessageSample> messages)
{
    qDebug() << "TopologySensor::write";

    if (clientConnection->state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "TopologySensor::write: no client connection; dropping all messages";
        return;
    }

    foreach(MessageSample msg, messages)
    {
        QByteArray arr = msg.toByteArray();
        qint64 dataLength = clientConnection->write(arr);

        if (dataLength > 0)
        {
            qDebug() << "TopologySensor::write: sent " << dataLength << " bytes of data";
        }
        else
        {
            qDebug() << "TopologySensor::write: error with writing to socket; lost msg: " << msg.toString();
        }
    }
}

QVector<MessageSample> TopologySensor::readAll()
{
    QVector<MessageSample> res = QVector<MessageSample>(msgQue);
    msgQue.clear();
    qDebug() << "TopologySensor::readAll: read" << res.size() << "messages";
    return res;
}

void TopologySensor::handleConnection()
{
    qDebug() << "TopologySensor: new connection";
    if (clientConnection != 0)
    {
        clientConnection->disconnectFromHost();
    }
    clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(deleteConnection()));
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(clientConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));
}

void TopologySensor::deleteConnection()
{
    qDebug() << "TopologySensor::deleteConnection: connection deleted";
    //delete clientConnection;
    //clientConnection = 0;
    //clientConnection->state();
}

void TopologySensor::readData()
{
    qDebug() << "TopologySensor::readData";

    QByteArray buff = clientConnection->readAll();
    QString s(buff);

    QStringList l = s.split("##", QString::SkipEmptyParts);

    for (int i = 0; i < l.size(); i++)
    {
        MessageSample m;
        QByteArray arr;
        arr.append(l[i]);
        if (m.fromByteArray(arr))
        {
            msgQue.push_back(m);
            qDebug() << "TopologySensor::readData: got msg:" << m.toString();
        }
        else
        {
            qDebug() << "TopologySensor::readData: got wrong message" << l[i];
        }
    }

    if (msgQue.size() > 0)
    {
        emit readyToRead();
    }
    else
    {
        qDebug() << "TopologySensor::readData: no correct message";
    }
}

void TopologySensor::handleError(QAbstractSocket::SocketError error)
{
    qWarning() << "TopologySensor: Error with socket: " << error;
}
