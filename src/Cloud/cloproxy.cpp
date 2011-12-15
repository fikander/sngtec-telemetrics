#include "cloproxy.h"
#include "../Sensors/devproxy.h"

CloProxy::CloProxy() {
    timer = new QTimer(this);
    askInterval = 15 * 1000; // 30 sekund
    connect(timer, SIGNAL(timeout()), this, SLOT(askServer()));
    timer->start(askInterval);
}


void CloProxy::connectDev(DevProxy *dv) {
    dev = dv;
}

void CloProxy::askServer() {
   qDebug() << "Ask server";
   if (!que.isEmpty()) {
    QString qs = que.dequeue();
    qDebug() << "Odebrano od device" << qs;
   }
   // Mozna cos popisac po dv
   QString msg = "ok";
   dev->ioDevice->write(msg.toAscii().data(), 2);
}

void CloProxy::queue(QString payload) {
    que.enqueue(payload);
}
