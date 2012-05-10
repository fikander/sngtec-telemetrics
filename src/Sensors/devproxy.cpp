#include "devproxy.h"
#include "Cloud/cloproxy.h"
#include "Sensors/devconnection.h"
#include "Message/message.h"
#include <QTcpSocket>

DevProxy::DevProxy() {}

DevProxy::DevProxy(Configurator* config) {
    ioDevice = config->giveDevice();
    bool sukces = connect(ioDevice, SIGNAL(readyToRead()), this, SLOT(readDevice()));


//    connect(ioDevice, SIGNAL(error(QAbstractSocket::SocketError)),
//            this, SLOT(displayError(QAbstractSocket::SocketError)));
}


void DevProxy::connectCloud(CloProxy *cl) {
    clo = cl;
    connect(this, SIGNAL(enque(Message)),
            clo, SLOT(queue(Message)));

}

void DevProxy::readDevice() {
    qDebug() << __PRETTY_FUNCTION__  << "Attempting read from device..";
    QVector<Message> payload = ioDevice->readAll();
    qDebug() << __PRETTY_FUNCTION__  << "Read data from device: " << payload[0].key << " " << payload[0].value;
    Message msg = payload[0];

    emit enque(msg);

    return;
}

void DevProxy::readCloud() {
   return;
}

void DevProxy::displayError(QAbstractSocket::SocketError) {
    qCritical() << "Socket fail";
    return;
}
