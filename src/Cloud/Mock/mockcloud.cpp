#include "mockcloud.h"
#include <QDebug>
#include <QVector>
#include "Message/message.h"

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

void MockCloud::write(QVector<Message> messages) {
    for(MessagesSet::const_iterator it = messages.begin();
        it != messages.end(); ++it){
        qDebug() <<  "Write of { key: " << it->key << ", value: " << it->value << ", time: "
                << it->timestamp << " }";
        
     }
}

QVector<Message> MockCloud::readAll() {
    QVector<Message> incoming;
    incoming.push_back(Message("alpha|cloud", "ok"));
    return incoming;
}

bool MockCloud::isBusy() {
    return false;
}

