#include "topologycloud.h"
#include "Message/message.h"
#include "Configurator/configurator.h"
#include <QString>
#include <QStringList>
#include <QByteArray>


//int TopologyCloud::max_messages = 200;
//int TopologyCloud::max_messages_size = 600;

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

    QObject::connect(&connection, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(&connection, SIGNAL(connectionClosed()), this, SLOT(disconnected()));
    QObject::connect(&connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    QObject::connect(&connection, SIGNAL(readyRead()), this, SLOT(readyRead()));

    connection.connectToHost(addres, port);
}

void TopologyCloud::connected() {
    qDebug() << "Connected to cloud";
    //writeConnected(messages_tosend);
    //messages_tosend.clear();
}

void TopologyCloud::disconnected() {
    qWarning() << "TopologyCloud::Disconnected from cloud";
    this->connect();
}

void TopologyCloud::error(QAbstractSocket::SocketError) {
    qWarning() << "TopologyCloud::Error during cloud connection: " << connection.error();
}

void TopologyCloud::write(QVector<Message> messages) {
//    if(connection.state() != QAbstractSocket::ConnectedState) {
//        messages_tosend += messages;

//        if(messages_tosend.size() > max_messages) {
//            qCritical() << "no connection and a lot of messages to send";
//            if(messages_tosend.size() > max_messages_size) {
//                qCritical() << "too many messages int queue, delteting";
//                messages_tosend.remove(0, max_messages_size / 2);
//            }
//        }
//    }
//    else {
//        writeConnected(messages);
//    }
    foreach(Message msg, messages)
    {
        QByteArray arr = msg.toByteArray();
        qint64 dataLength = connection.write(arr);
        qDebug() << "TopologyCloud::write: sent " << dataLength << " bytes of data";
    }
}

//void TopologyCloud::writeConnected(QVector<Message> messages) {
//    connection.write(QByteArray::number(messages.size()));
//    foreach(const Message message, messages) {
//        connection.write(message.key.toAscii());
//        connection.write(message.value.toAscii());
//        connection.write(message.timestamp.toString().toAscii());
//    }
//}

QVector<Message> TopologyCloud::readAll() {
    //return QVector<Message>();
    QVector<Message> res = QVector<Message>(msgQue);
    msgQue.clear();
    return res;
}

bool TopologyCloud::isBusy() {
    return false;
}

void TopologyCloud::readyRead() {
//    if(connection.canReadLine()) {
//        if(messages_toread > 0) {
//            read_values[read_state] = connection.readLine();
//            if(read_state == READ_timestamp) {
//                messages_readed.push_back(
//                    Message(read_values[READ_key],
//                        read_values[READ_value],
//                        QDateTime::fromString(read_values[READ_timestamp])));
//                --messages_toread;
//                if(messages_toread == 0) {
//                    emit orderReceived(messages_readed);
//                    messages_readed.clear();
//                }
//            }
//            read_state = (read_state + 1) % READ_max;
//        }
//        else {
//            messages_toread = connection.readLine().toInt();
//            read_state = READ_key;
//        }
//        readyRead();
//    }
    QByteArray buff = connection.readAll();
    QString s(buff);

    QStringList l = s.split("##", QString::SkipEmptyParts);

    for (int i = 0; i < l.size(); i++)
    {
        Message m;
        QByteArray arr;
        arr.append(l[i]);
        m.fromByteArray(arr);
        msgQue.push_back(m);
    }

    emit orderReceived(msgQue);
    msgQue.clear();
}
