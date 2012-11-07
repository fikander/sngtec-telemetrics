#include "mockcloud.h"
#include <QDebug>
#include <QVector>
#include "Message/message.h"

MockCloud::MockCloud() {
    QObject::connect(&receive, SIGNAL(timeout()), this, SLOT(receiveFakeData()));
    receive.start(60 * 1000);
}

CloConnection* MockCloud::create(Configurator*) {
    return new MockCloud();
}

CloConnection* MockCloud::clone(Configurator*) {
    return new MockCloud();
}

MockCloud::~MockCloud() {
}

void MockCloud::connect() {
}

void MockCloud::write(QVector<MessageSample> messages) {
    for(MessagesSet::const_iterator it = messages.begin();
        it != messages.end(); ++it){
        qDebug() <<  "Write { key: " << it->key << ", value: " << it->value << ", time: "
               << it->timestamp << " }";
        
     }
}

QVector<MessageSample> MockCloud::readAll() {
    QVector<MessageSample> incoming;
    incoming.push_back(MessageSample("alpha|cloud", "ok"));
    return incoming;
}

void MockCloud::receiveFakeData() {
    QVector<MessageSample> incoming;
    incoming.push_back(MessageSample("alpha|cloud", "ok"));
    emit orderReceived(incoming);
}

bool MockCloud::isBusy() {
    return false;
}

