#ifndef CLOUD_H
#define CLOUD_H

#include <QObject>

#include "KeyValueMap.h"
#include "Message/message.h"

class Cloud : public QObject
{
    Q_OBJECT

public:
    Cloud(KeyValueMap &config);
    virtual ~Cloud();

    virtual void connect() = 0;

    bool connected() const
        { return m_connected; }
    bool initialised() const
        { return m_initialised; }
    QString getName() const
        { return name; }

public slots:
    /**
     * Send message (sample or event - events may be dispatched immediately) to the cloud.
     * Triggered by sensors.
     */
    virtual void send(QSharedPointer<Message> payload) = 0;

signals:
    /**
     * Received command or sample - update of parameter value - from cloud
     * Consumed by sensors.
     */
    void received(QSharedPointer<Message> payload);

protected:
    bool m_connected;
    bool m_initialised;
    QString name;
};

#endif // CLOUD_H
