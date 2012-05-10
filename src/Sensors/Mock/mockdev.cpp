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
    qDebug() << "Mock sensor number:" << number << "sending fake data";
    emit readyToRead2();

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
