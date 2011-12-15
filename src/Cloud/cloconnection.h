#ifndef CLOCONNECTION_H
#define CLOCONNECTION_H

#include <QObject>
#include <QVector>


class Message;
class Configurator;

// Interface for various connection types with cloud
class CloConnection : public QObject {
    Q_OBJECT

public:
    virtual CloConnection* create(Configurator &config) = 0;
    virtual CloConnection* clone(Configurator &config) = 0;
    virtual ~CloConnection() = 0;
    virtual void write(QVector<Message>) = 0;
    virtual QVector<Message> readAll() = 0;
    virtual bool isBusy() = 0;

// Unused so far
signals:
    void readyToRead();
    void readyToWrite();
};

#endif // CLOCONNECTION_H
