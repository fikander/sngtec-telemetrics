#include "topologysensor.h"

int TopologySensor::max_messages = 200;
int TopologySensor::max_messages_size = 600;

TopologySensor::TopologySensor(Configurator * config, int no) :
    tcpServer(0), clientConnection(0), port(0), messages_toread(0)
{
    port = config->deviceTranslate(no, "port").toInt();

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleConnection()));
    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, port)) {
        qCritical() << "Unable to start the server: " << tcpServer->errorString();
        return;
    }
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

void TopologySensor::write(QVector<Message> msg)
{
    msgToSend += msg;

    if(msgToSend.size() > max_messages) {
        qCritical() << "TopologySensor::write: too lot of messages to send, removing the oldest";
        msgToSend.remove(0, msgToSend.size() - max_messages);
    }

    if(clientConnection == 0)
    {
        qWarning() << "TopologySensor::write: disconnected";

    }
    else if (clientConnection->state() != QAbstractSocket::ConnectedState)
    {
        qWarning() << "TopologySensor::write: error with socket";
    }
    else
    {
        sendMsg();
    }
}

void TopologySensor::sendMsg()
{
    clientConnection->write(QByteArray::number(msgToSend.size()));
    foreach(const Message msg, msgToSend) {
        clientConnection->write(msg.key.toAscii());
        clientConnection->write(msg.value.toAscii());
        clientConnection->write(msg.timestamp.toString().toAscii());
    }
}

QVector<Message> TopologySensor::readAll()
{
    QVector<Message> res = QVector<Message>(msgQue);
    msgQue.clear();
    return res;
}

void TopologySensor::handleConnection()
{
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
    if(clientConnection->canReadLine())
    {
        if(messages_toread > 0)
        {
            read_values[read_state] = clientConnection->readLine();
            if(read_state == READ_timestamp)
            {
                msgQue.push_back(Message(read_values[READ_key], read_values[READ_value],
                                        QDateTime::fromString(read_values[READ_timestamp])));
                --messages_toread;
                if(messages_toread == 0)
                {
                    emit readyToRead();
                }
            }
            read_state = (read_state + 1) % READ_max;
        }
        else {
            messages_toread = clientConnection->readLine().toInt();
            read_state = READ_key;
        }
    }
}

void TopologySensor::handleError(QAbstractSocket::SocketError error)
{
    qWarning() << "Error with socket: " << error;
}
