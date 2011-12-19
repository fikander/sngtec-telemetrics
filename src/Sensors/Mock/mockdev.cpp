#include "mockdev.h"
#include <QVector>
#include <QDebug>
#include "src/Message/message.h"

MockDev::MockDev() {}

MockDev::MockDev(Configurator *config) {
    timer = new QTimer(this);
    askInterval = 5 * 1000; // 15 sekund
    connect(timer, SIGNAL(timeout()), this, SLOT(fakeSignal()));
    qDebug() << "pusczam mock dev";
    timer->start(askInterval);
 }

DevConnection* MockDev::create(Configurator *config) {
    return new MockDev(config);
}

DevConnection* MockDev::clone(Configurator *config) {
    return new MockDev(config);
}

MockDev::~MockDev() {
}

void MockDev::fakeSignal() {
    qDebug() << "Mock dev received fake data";
    emit readyToRead();
}


void MockDev::write(QVector<Message>) {
    return;
}

QVector<Message> MockDev::readAll() {
    QVector<Message> res;
    Message m("A", "B");
    res.push_back(m);

    return res;
}
