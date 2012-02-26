#ifndef MOCKDEV_H
#define MOCKDEV_H

#include "Sensors/devconnection.h"
#include <QTimer>

class MockDev : public DevConnection
{
Q_OBJECT
private:
    MockDev(Configurator *config, int no);
    QTimer *timer;
    int askInterval;
    int number;
public:
    MockDev();
    virtual DevConnection* create(Configurator *config, int no);
    virtual DevConnection* clone(Configurator *config, int no);
    virtual ~MockDev();

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();

signals:
    void readyToRead();

public slots:
    void fakeSignal();

};

#endif // MOCKDEV_H
