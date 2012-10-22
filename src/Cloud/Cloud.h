#ifndef CLOUD_H
#define CLOUD_H

#include <QObject>

#include "Message/message.h"

class Cloud : public QObject
{
    Q_OBJECT

public:
    Cloud();
    virtual ~Cloud();

    virtual void connect() = 0;

    bool connected() { return m_connected; }

public slots:
    /*
     * Send to cloud
     */
    virtual void send(QSharedPointer<Message> payload) = 0;

signals:
    /*
     * Received from cloud
     */
    void received(QSharedPointer<Message> payload);

protected:
    bool m_connected;

};

#endif // CLOUD_H
