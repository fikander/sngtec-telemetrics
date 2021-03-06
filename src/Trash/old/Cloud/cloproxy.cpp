#include "cloproxy.h"
#include "Sensors/devproxy.h"
#include "Cloud/cloconnection.h"
#include "Message/message.h"
#include "Sensors/devconnection.h"
#include "LogSender/logsender.h"
#include "LogSender/HTTPLogSender/httplogsender.h"


CloProxy::CloProxy(Configurator *config) {
    configurator = config;
    timer = new QTimer(this);
    askInterval = 5 * 1000; // 15 sekund
    connect(timer, SIGNAL(timeout()), this, SLOT(askServer()));
    timer->start(askInterval);

    ioDevice = config->giveCloud();
    ioDevice->connect();
    sender = new HTTPLogSender();
    connect(sender, SIGNAL(statusUpdate(MessageSample)), this, SLOT(senderResponse(MessageSample)));
    connect(config->giveCloud(), SIGNAL(orderReceived(QVector<MessageSample>)), this, SLOT(receiveServerMessages(QVector<MessageSample> )));
}


void CloProxy::connectDev(DevProxy *dv) {
    devList.push_back(dv);
    // dev = dv;
}

void CloProxy::dispatchMessage(MessageSample m) {
    DevProxy *dev;
    // Extract name from message key
    QString name = m.key.section('|', 0, 0);

    if (name == configurator->logPushCommand) {
        QString address = m.value;
        sender->sendLogs(address,
                         Logger::getInstance()->giveLogs());
        return;
    }

    // Find the mapping in the devices table
    if (configurator->devNamesToNumbers.contains(name)) {
        dev = devList[configurator->devNamesToNumbers[name]];
        // Prepare some payload
        QVector<MessageSample> payload;
        m.key = m.key.section('|', 1);
        payload.push_back(m);
        dev->ioDevice->write(payload);
        return;
    } else {
        qWarning() << "Ignoring unexpected message:" << m.key << " " << m.value;
    }
    //qDebug() << __PRETTY_FUNCTION__  << "Sent to device number:" << configurator->devNamesToNumbers[name] << " " << m.key << ";" << m.value;
}

void CloProxy::askServer() {
   //qDebug() << __PRETTY_FUNCTION__ << "Asking server..";

   // sensors -> cloud
   while (!que.isEmpty()) {

       // qs.value = "100";
       //qDebug() << __PRETTY_FUNCTION__ << "Trying to send to server: " << qs.value;

       if (!ioDevice->isBusy()) {
           MessageSample qs = que.dequeue();
           QVector<MessageSample> msgs;
           msgs.push_back(qs);
           //qDebug() << __PRETTY_FUNCTION__ << "Sending to server..";
           ioDevice->write(msgs);
       } else {
           //qDebug() << __PRETTY_FUNCTION__ << "Server busy.";
       }
   }


   // cloud -> sensors
   // Some stuff should be downloaded here..


   /* Replaced for signals
   QVector<Message> incoming;
   incoming = ioDevice->readAll();
   for (int i = 0; i < incoming.size(); i++) {
       //qDebug() << __PRETTY_FUNCTION__ << "Read from cloud" << incoming[i].key << " " << incoming[i].value;
       dispatchMessage(incoming[i]);
   }
   */
}

void CloProxy::receiveServerMessages(QVector<MessageSample> incoming) {
   for (int i = 0; i < incoming.size(); i++) {
       //qDebug() << __PRETTY_FUNCTION__ << "Read from cloud" << incoming[i].key << " " << incoming[i].value;
       dispatchMessage(incoming[i]);
   }
}

void CloProxy::queue(MessageSample payload) {
    que.enqueue(payload);
}

void CloProxy::senderResponse(MessageSample payload) {
    payload.key = configurator->logPushResponse;
    que.enqueue(payload);
}

