#include "cloproxy.h"
#include "src/Sensors/devproxy.h"
#include "src/Cloud/cloconnection.h"

CloProxy::CloProxy(Configurator *config) {
    timer = new QTimer(this);
    askInterval = 15 * 1000; // 15 sekund
    connect(timer, SIGNAL(timeout()), this, SLOT(askServer()));
    timer->start(askInterval);

    ioDevice = config->giveCloud();
}


void CloProxy::connectDev(DevProxy *dv) {
    dev = dv;
}

void CloProxy::askServer() {
   qDebug() << "Asking server..";

   if (!que.isEmpty()) {
    Message qs = que.dequeue();
    // qs.value = "100";
    qDebug() << "Trying to send to server: " << qs.value;

    if (!ioDevice->isBusy()) {
        QVector<Message> msgs;
        msgs.push_back(qs);
        qDebug() << "Sending to server..";
        ioDevice->write(msgs);
    } else {
        qDebug() << "Server busy.";
    }
}


   Message msg("status", "ok");
   QVector<Message> wiadomosci;
   wiadomosci.push_back(msg);
   dev->ioDevice->write(wiadomosci);
}

void CloProxy::queue(Message payload) {
    que.enqueue(payload);
}
