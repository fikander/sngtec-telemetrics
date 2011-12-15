#ifndef CLOCONNECTION_H
#define CLOCONNECTION_H

#include <QObject>
#include "../message.h"
#include "../configurator.h"


// Interface for various connection types with cloud
class CloConnection : public QObject {
    Q_OBJECT

public:
    virtual CloConnection* create(Configurator &config);
    virtual CloConnection* clone(Configurator &config);
    virtual ~CloConnection();
    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();

// Unused so far
signals:
    void readyToRead();
};

#endif // CLOCONNECTION_H
