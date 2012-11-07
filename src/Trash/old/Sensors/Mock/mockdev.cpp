#include "mockdev.h"
#include <QVector>
#include <QDebug>
#include "Message/message.h"

MockDev::MockDev() {}

MockDev::MockDev(Configurator *config, int no) {
    number = no;
    timer = new QTimer(this);
    askInterval = 5 * 1000; // 5 sekund
    connect(timer, SIGNAL(timeout()), this, SLOT(fakeSignal()));
    timer->start(askInterval);
 }

DevConnection* MockDev::create(Configurator *config, int no) {
    return new MockDev(config, no);
}

DevConnection* MockDev::clone(Configurator *config, int no) {
    return new MockDev(config, no);
}

MockDev::~MockDev() {
}

void MockDev::fakeSignal() {
    //qDebug() << __PRETTY_FUNCTION__ << "Mock sensor number:" << number << "sending fake data";
    emit readyToRead();

}


void MockDev::write(QVector<MessageSample> messages) {
    qDebug() << "Mock: writing messages";
    foreach (MessageSample m, messages) {
        qDebug() << "Got message:" << m.key << "," << m.value;
    }
    return;
}

QVector<MessageSample> MockDev::readAll() {
    QVector<MessageSample> res;
    MessageSample m("A", "B");
    res.push_back(m);

    return res;
}
