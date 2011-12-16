#include "devproxy.h"
#include "src/Cloud/cloproxy.h"
#include <QTcpSocket>

//
DevProxy::DevProxy() {
    // Device
    ioDevice = new QTcpSocket(this);

    connect(ioDevice, SIGNAL(readyRead()), this, SLOT(readDevice()));
    connect(ioDevice, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    ioDevice->connectToHost("localhost", 7171);

}

void DevProxy::connectCloud(CloProxy *cl) {
    // Proxy chmury
    clo = cl;
    connect(this, SIGNAL(enque(QString)),
            clo, SLOT(queue(QString)));

}

void DevProxy::readDevice() {
    // czytaj z socketa
    QString payload = ioDevice->readAll();
    qDebug() << "Czytaj z socketa: " << payload;

    // wyslij chmurze
    emit enque(payload);

    return;
}

void DevProxy::readCloud() {
   return;
}

void DevProxy::displayError(QAbstractSocket::SocketError) {
    qDebug() << "Socket fail";
    return;
}
