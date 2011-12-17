#include "mockdev.h"
#include <QVector>
#include "src/Message/message.h"

//MockDev::MockDev() {
//}

DevConnection* MockDev::create(Configurator *config) {
    return new MockDev();
}

DevConnection* MockDev::clone(Configurator *config) {
    return new MockDev();
}

MockDev::~MockDev() {
}


void MockDev::write(QVector<Message>) {
    return;
}

QVector<Message> MockDev::readAll() {
    QVector<Message> res;
    Message m("Ok", "Ok");
    res.push_back(m);

    return res;
}
