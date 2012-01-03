#ifndef MOCKDEV_H
#define MOCKDEV_H

#include "src/Sensors/devconnection.h"
#include <QTimer>

class MockDev : public DevConnection
{
Q_OBJECT
private:
    MockDev(Configurator *config);
    QTimer *timer;
    int askInterval;

public:
    MockDev();
    virtual DevConnection* create(Configurator *config);
    virtual DevConnection* clone(Configurator *config);
    virtual ~MockDev();

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();

signals:
    void readyToRead();

public slots:
    void fakeSignal();

};

#endif // MOCKDEV_H
