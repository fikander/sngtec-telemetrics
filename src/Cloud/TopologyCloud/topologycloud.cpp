#include "topologycloud.h"
#include "Message/message.h"
#include "Configurator/configurator.h"
#include <QString>
#include <QStringList>
#include <QByteArray>

TopologyCloud::TopologyCloud(Configurator *config): addres(config->getCloudAddress()), port(config->getCloudPort()){
}

TopologyCloud::TopologyCloud(QString addr, int port): addres(addr), port(port){
}

CloConnection* TopologyCloud::create(Configurator* configurator) {
    return new TopologyCloud(configurator);
}

CloConnection* TopologyCloud::clone(Configurator* configurator) {
    return new TopologyCloud(configurator);
}

TopologyCloud::~TopologyCloud() {}

void TopologyCloud::connect() {
    qDebug() << "TopologyCloud::connect";

    QObject::connect(&connection, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(&connection, SIGNAL(connectionClosed()), this, SLOT(disconnected()));
    QObject::connect(&connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    QObject::connect(&connection, SIGNAL(readyRead()), this, SLOT(readyRead()));

    connection.connectToHost(addres, port);
}

void TopologyCloud::connected() {
    qDebug() << "TopologyCloud: Connected to cloud";
}

void TopologyCloud::disconnected() {
    qWarning() << "TopologyCloud::Disconnected from cloud";
    this->connect();
}

void TopologyCloud::error(QAbstractSocket::SocketError) {
    qWarning() << "TopologyCloud::Error during cloud connection: " << connection.error();
}

void TopologyCloud::write(QVector<MessageSample> messages)
{
    qDebug() << "TopologyCloud::write";

    if (connection.state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "TopologyCloud::write: no connection; dropping all messages";
        //connect();
        return;
    }

    foreach(MessageSample msg, messages)
    {
        QByteArray arr = msg.toByteArray();
        qint64 dataLength = connection.write(arr);

        if (dataLength > 0)
        {
            qDebug() << "TopologyCloud::write: sent " << dataLength << " bytes of data";
        }
        else
        {
            qDebug() << "TopologyCloud::write: error with writing to socket; lost msg: " << msg.toString();
        }
    }
}


QVector<MessageSample> TopologyCloud::readAll() {
    QVector<MessageSample> res = QVector<MessageSample>(msgQue);
    msgQue.clear();
    return res;
}


bool TopologyCloud::isBusy() {
    qDebug() << "TopologyCloud::isBusy";
    return (connection.state() != QTcpSocket::ConnectedState);
}


void TopologyCloud::readyRead() {
    QByteArray buff = connection.readAll();
    QString s(buff);

    QStringList l = s.split("##", QString::SkipEmptyParts);

    for (int i = 0; i < l.size(); i++)
    {
        MessageSample m;
        QByteArray arr;
        arr.append(l[i]);
        m.fromByteArray(arr);
        msgQue.push_back(m);
    }

    emit orderReceived(msgQue);
    msgQue.clear();
}
