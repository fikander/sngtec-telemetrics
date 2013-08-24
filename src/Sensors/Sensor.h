#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>

#include "KeyValueMap.h"
#include "Message/message.h"

class Sensor : public QObject
{
    Q_OBJECT
public:
    Sensor(KeyValueMap &config, QObject *parent = 0);

    virtual ~Sensor();

    virtual int connect() = 0;

    bool connected() const
        { return m_connected; }
    bool initialised() const
        { return m_initialised; }
    QString getName() const
        { return name; }

public slots:
    /*
     * Send to sensor
     */
    virtual void send(QSharedPointer<Message> payload) = 0;

signals:
    /*
     * Received from sensor
     */
    void received(QSharedPointer<Message> payload);

protected:
    bool m_connected;
    bool m_initialised;
    QString name;
};

#endif // SENSOR_H
