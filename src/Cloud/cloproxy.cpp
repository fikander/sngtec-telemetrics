#include "cloproxy.h"
#include "src/Sensors/devproxy.h"

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
    Message qs = que.dequeue();
    qDebug() << "Odebrano od device" << qs.value;
   }
   // Mozna cos popisac po dv
   QString msg = "ok";
   dev->ioDevice->write(msg.toAscii().data(), 2);
}

void CloProxy::queue(Message payload) {
    que.enqueue(payload);
}
