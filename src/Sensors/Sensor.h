#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>

#include "Message/message.h"

class Sensor : public QObject
{
    Q_OBJECT
public:
    Sensor();

    virtual ~Sensor();

    virtual void connect() = 0;

    bool connected() { return m_connected; }

public slots:
    /*
     * Send to sensor
     */
    virtual void send(Message &payload) = 0;

signals:
    /*
     * Received from sensor
     */
    void received(Message &payload);

protected:
    bool m_connected;
};

#endif // SENSOR_H
