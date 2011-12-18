#include "devproxy.h"
#include "src/Cloud/cloproxy.h"
#include "src/Sensors/devconnection.h"
#include "src/Message/message.h"
#include <QTcpSocket>


//
DevProxy::DevProxy() {}

DevProxy::DevProxy(Configurator* config) {
    ioDevice = config->giveDevice();
    connect(ioDevice, SIGNAL(readyToRead()), this, SLOT(readDevice()));

//    connect(ioDevice, SIGNAL(error(QAbstractSocket::SocketError)),
//            this, SLOT(displayError(QAbstractSocket::SocketError)));
}


void DevProxy::connectCloud(CloProxy *cl) {
    clo = cl;
    connect(this, SIGNAL(enque(Message)),
            clo, SLOT(queue(Message)));

}

void DevProxy::readDevice() {
    QVector<Message> payload = ioDevice->readAll();
    qDebug() << "Read from socket: " << payload[0].key << " " << payload[0].value;
    Message msg = payload[0];

    emit enque(msg);

    return;
}

void DevProxy::readCloud() {
   return;
}

void DevProxy::displayError(QAbstractSocket::SocketError) {
    qDebug() << "Socket fail";
    return;
}
