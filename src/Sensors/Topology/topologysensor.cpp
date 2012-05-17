#include "topologysensor.h"
#include <QString>
#include <QStringList>
#include <QByteArray>

//int TopologySensor::max_messages = 200;
//int TopologySensor::max_messages_size = 600;

TopologySensor::TopologySensor(Configurator * config, int no) :
    tcpServer(0), clientConnection(0), port(0)
{
    port = config->deviceTranslate(no, "port").toInt();

    tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleConnection()));

    if (!tcpServer->listen(QHostAddress::Any, port)) {
        qCritical() << "Unable to start the server: " << tcpServer->errorString();
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

void TopologySensor::write(QVector<Message> messages)
{
    qDebug() << "TopologySensor:"<< "write";
    foreach(Message msg, messages)
    {
        QByteArray arr = msg.toByteArray();
        qint64 dataLength = clientConnection->write(arr);
        qDebug() << "TopologyCloud::write: sent " << dataLength << " bytes of data";
    }
//    qDebug() << "TopologySensor:"<< "write";
//    msgToSend += msg;

//    if(msgToSend.size() > max_messages) {
//        qCritical() << "TopologySensor::write: too lot of messages to send, removing the oldest";
//        msgToSend.remove(0, msgToSend.size() - max_messages);
//    }

//    if(clientConnection == 0)
//    {
//        qWarning() << "TopologySensor::write: disconnected";

//    }
//    else if (clientConnection->state() != QAbstractSocket::ConnectedState)
//    {
//        qWarning() << "TopologySensor::write: error with socket";
//    }
//    else
//    {
//        sendMsg();
//    }
}

//void TopologySensor::sendMsg()
//{
//    clientConnection->write(QByteArray::number(msgToSend.size()));
//    foreach(const Message msg, msgToSend) {
//        clientConnection->write(msg.key.toAscii());
//        clientConnection->write(msg.value.toAscii());
//        clientConnection->write(msg.timestamp.toString().toAscii());
//    }
//}

QVector<Message> TopologySensor::readAll()
{
    QVector<Message> res = QVector<Message>(msgQue);
    msgQue.clear();
    qDebug() << __PRETTY_FUNCTION__ << " read messages: " << res.size();
    return res;
}

void TopologySensor::handleConnection()
{
    qDebug() << "TopologySensor:"<< "new connection";
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
    delete clientConnection;
    clientConnection = 0;
}

void TopologySensor::readData()
{
    qDebug() << "TopologySensor:"<< "readData";

//    qDebug() << __PRETTY_FUNCTION__ << "readData called";
//    if(true || clientConnection->canReadLine()) // SHORT CIRCUITED FOR DEBUG ONLY
//    {
//        qDebug() << __PRETTY_FUNCTION__ << "can readLine";

//        if(true || messages_toread > 0) // SHORT CIRCUITED FOR DEBUG ONLY
//        {
//            qDebug() << __PRETTY_FUNCTION__ << "messages to read";

//            read_values[read_state] = clientConnection->readLine();
//            if(true || read_state == READ_timestamp)// SHORT CIRCUITED FOR DEBUG ONLY
//            {
//                qDebug() << __PRETTY_FUNCTION__ << "checked timestamp";

//                msgQue.push_back(Message(read_values[READ_key], read_values[READ_value],
//                                        QDateTime::fromString(read_values[READ_timestamp])));
//                //--messages_toread; // DEBUG ONLY
//                qDebug() << __PRETTY_FUNCTION__ << messages_toread << "messages left to read";

//                if(messages_toread == 0)
//                {
//                    emit readyToRead();
//                }
//            }
//            read_state = (read_state + 1) % READ_max;
//        }
//        else {
//            messages_toread = clientConnection->readLine().toInt();
//            qDebug() << __PRETTY_FUNCTION__ << messages_toread << "messages left to read";

//            read_state = READ_key;
//        }
//    }
    QByteArray buff = clientConnection->readAll();
    QString s(buff);

    QStringList l = s.split("##", QString::SkipEmptyParts);

    for (int i = 0; i < l.size(); i++)
    {
        Message m;
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

    emit readyToRead();
}

void TopologySensor::handleError(QAbstractSocket::SocketError error)
{
    qWarning() << "Error with socket: " << error;
}
